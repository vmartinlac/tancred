#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>
#include <opencv2/imgcodecs.hpp>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/TypedReaderCallback.h>
#include <unistd.h>
#include <fcntl.h>
#include <ImageMessage.h>
#include <MotorsMessage.h>

using ClockType = std::chrono::high_resolution_clock;

struct Common
{
    std::string root_directory;
    std::mutex mutex;
    std::ofstream image_file;
    std::ofstream motors_file;
    bool in_sequence;
    int sequence;
    ClockType::time_point time_reference;
};

class ImageCallback : public yarp::os::TypedReaderCallback<ImageMessage>
{
public:

    ImageCallback(Common* c)
    {
        myCommon = c;
    }

    ImageCallback(const ImageCallback& o) = delete;

    void onRead(ImageMessage& msg) override
    {
        bool in_sequence = false;
        int sequence = 0;
        ClockType::time_point t0;

        std::cout << "Image message" << std::endl;

        myCommon->mutex.lock();
        in_sequence = myCommon->in_sequence;
        sequence = myCommon->sequence;
        t0 = myCommon->time_reference;
        myCommon->mutex.unlock();

        const int64_t dt = std::chrono::duration_cast<std::chrono::milliseconds>( ClockType::now() - t0 ).count();

        if(in_sequence)
        {
            std::vector<uint8_t> encoded;
            std::string image_path;
            bool ok = true;
            const char* err = "";
            int fd = -1;

            // encode image.

            if(ok)
            {
                cv::Mat1b image(msg.height, msg.width, reinterpret_cast<uint8_t*>(msg.data.data()));

                ok = cv::imencode(".png", image, encoded);
                err = "Could not encode image!";
            }

            // save image on filesystem.

            if(ok)
            {
                std::stringstream fname;
                fname << myCommon->root_directory << '/' << std::setw(12) << std::setfill('0') << msg.frameid << ".png";
                image_path = fname.str();
            }

            if(ok)
            {
                std::cout << "Saving to: " << image_path << std::endl;
                fd = open(image_path.c_str(), O_WRONLY|O_SYNC|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
                ok = (fd >= 0);
                err = "Could not open image file for writing!";
            }

            if(ok)
            {
                ok = ( write(fd, encoded.data(), encoded.size()) == encoded.size() );
                err = "Could not write image on filesystem!";
            }

            if( fd >= 0 )
            {
                close(fd);
                fd = -1;
            }

            // record image in database.

            if(ok)
            {
                myCommon->image_file << sequence << " " << dt << " " << msg.frameid << std::endl;
            }

            if(ok == false)
            {
                std::cout << err << std::endl;
                exit(1);
            }
        }
    }

protected:

    Common* myCommon;
};

class MotorsCallback : public yarp::os::TypedReaderCallback<MotorsMessage>
{
public:

    MotorsCallback(Common* c)
    {
        myCommon = c;
    }

    MotorsCallback(const MotorsCallback& o) = delete;

    void onRead(MotorsMessage& msg) override
    {
        std::cout << "MOTORS message" << std::endl;

        if(msg.forward)
        {
            myCommon->mutex.lock();
            if(myCommon->in_sequence == false)
            {
                myCommon->in_sequence = true;
                myCommon->time_reference = ClockType::now();
            }
            myCommon->mutex.unlock();

            const int64_t dt = std::chrono::duration_cast<std::chrono::milliseconds>( ClockType::now() - myCommon->time_reference ).count();

            myCommon->motors_file << myCommon->sequence << " " <<  dt << " " << msg.steering << std::endl;
        }
        else if( myCommon->in_sequence )
        {
            myCommon->mutex.lock();
            myCommon->in_sequence = false;
            myCommon->sequence++;
            myCommon->mutex.unlock();
        }
    }

protected:

    Common* myCommon;
};

int main(int num_args, char** args)
{
    bool ok = true;
    const char* err = "";

    yarp::os::Network yarp;
    yarp::os::BufferedPort<ImageMessage> image_port;
    yarp::os::BufferedPort<MotorsMessage> motors_port;
    Common common;
    ImageCallback image_callback(&common);
    MotorsCallback motors_callback(&common);

    if(ok)
    {
        ok = (num_args == 2);
        err = "Please provide the path to output directory on command line!";
    }

    if(ok)
    {
        const std::string path_image = std::string(args[1]) + "/image_log.txt";
        const std::string path_motors = std::string(args[1]) + "/motors_log.txt";

        common.image_file.open(path_image.c_str());
        common.motors_file.open(path_motors.c_str());

        common.in_sequence = false;
        common.sequence = 0;
        common.root_directory = args[1];

        ok = common.image_file.is_open() && common.motors_file.is_open();
        err = "Could not open log files!";
    }

    if(ok)
    {
        image_port.open("/recorder/camera_input");
        motors_port.open("/recorder/motors_input");

        image_port.useCallback(image_callback);
        motors_port.useCallback(motors_callback);
    }

    if(ok)
    {
        std::cout << "Press ENTER to quit" << std::endl;
        getchar();
    }

    if(ok)
    {
        image_port.close();
        motors_port.close();
        common.image_file.close();
        common.motors_file.close();
    }

    if(ok == false)
    {
        std::cout << err << std::endl;
        exit(1);
    }

    return 0;
}

