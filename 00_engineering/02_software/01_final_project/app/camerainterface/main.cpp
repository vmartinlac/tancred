#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <librealsense2/rs.hpp>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <ImageMessage.h>
#include "constants.h"

class VideoInput
{
public:

    void init()
    {
        bool found = false;

        std::vector<rs2::sensor> sensors = myContext.query_all_sensors();

        for(int i=0; found==false && i<sensors.size(); i++)
        {
            rs2::sensor s = sensors[i];

            std::vector<rs2::stream_profile> profiles = s.get_stream_profiles();

            for(int j=0; found == false && j<profiles.size(); j++)
            {
                rs2::stream_profile p = profiles[j];

                if( p.is<rs2::video_stream_profile>() && p.fps() == VIDEO_FPS && p.format() == RS2_FORMAT_YUYV && p.stream_type() == RS2_STREAM_COLOR )
                {
                    rs2::video_stream_profile vp = p.as<rs2::video_stream_profile>();
                    
                    if( vp.width() == VIDEO_IMAGE_WIDTH && vp.height() == VIDEO_IMAGE_HEIGHT )
                    {
                        found = true;
                        mySensor = s;
                        myProfile = p;
                    }

                    std::cout << ( (found) ? "Found " : "Skipping " ) << vp.width() << "*" << vp.height() << "@" << p.fps() << std::endl;
                }
            }
        }

        if(found == false)
        {
            std::cerr << "No suitable sensor/stream_profile detected!" << std::endl;
            exit(1);
        }

        mySensor.open(myProfile);
        mySensor.start(myFrameQueue);
    }

    rs2::frame_queue& refFrameQueue()
    {
        return myFrameQueue;
    }

    void finalize()
    {
        mySensor.stop();
        mySensor.close();
    }

protected:

    rs2::context myContext;
    rs2::sensor mySensor; 
    rs2::stream_profile myProfile;
    rs2::frame_queue myFrameQueue;
};

void send_raw_image(rs2::video_frame image, yarp::os::BufferedPort<ImageMessage>& port)
{
    const char* data = reinterpret_cast<const char*>(image.get_data());

    ImageMessage& msg = port.prepare();

    msg.frameid = image.get_frame_number();
    msg.timestamp = image.get_timestamp();
    msg.width = VIDEO_IMAGE_WIDTH;
    msg.height = VIDEO_IMAGE_HEIGHT;
    msg.format = ImageMessage::FORMAT_GRAYSCALE8;
    msg.data.resize(VIDEO_IMAGE_WIDTH*VIDEO_IMAGE_HEIGHT);
    for(int i=0; i<VIDEO_IMAGE_HEIGHT; i++)
    {
        for(int j=0; j<VIDEO_IMAGE_WIDTH; j++)
        {
            msg.data[VIDEO_IMAGE_WIDTH*i+j] = data[2*i*VIDEO_IMAGE_WIDTH+2*j];
        }
    }

    port.write();
}

int main(int num_args, char** args)
{
    // initialize realsense2.

    VideoInput input;
    input.init();

    // initialize VPX.

    /*
    Codec codec;
    codec.init();
    */

    // initialize YARP.

    yarp::os::Network yarp;
    yarp::os::BufferedPort<ImageMessage> raw_port;
    //yarp::os::BufferedPort<ImageMessage> compressed_port;

    raw_port.open("/camerainterface/output/image");
    //compressed_port.open("/camerainterface/compressed_image_output");

    // thread allowing the user to request the program to quit.

    volatile bool quit = false;
    auto thread_proc = [] (volatile bool* q)
    {
        std::cout << "Press ENTER to quit." << std::endl;
        getchar();
        *q = true;
    };

    std::thread thread(thread_proc, &quit);

    while(quit == false)
    {
        rs2::frame frame;

        try
        {
            frame = input.refFrameQueue().wait_for_frame(100);
        }
        catch(const rs2::error& e)
        {
            std::cout << "No frame" << std::endl;
        }

        if(frame && frame.is<rs2::video_frame>())
        {
            rs2::video_frame video_frame = frame.as<rs2::video_frame>();

            if(video_frame)
            {
                if( video_frame.get_width() != VIDEO_IMAGE_WIDTH || video_frame.get_height() != VIDEO_IMAGE_HEIGHT || video_frame.get_profile().format() != RS2_FORMAT_YUYV )
                {
                    std::cerr << "Internal error!" << std::endl;
                    exit(1);
                }

                send_raw_image(video_frame, raw_port);
                //codec.send(video_frame, compressed_port);
            }
        }
    }

    thread.join();
    raw_port.close();
    //compressed_port.close();
    //codec.finalize();
    input.finalize();

    return 0;
}

