#include <librealsense2/rs.hpp>
#include <iostream>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <ImageMessage.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

int main(int num_args, char** args)
{
    yarp::os::Network yarp;
    yarp::os::BufferedPort<ImageMessage> port;

    port.open("/camera");

    rs2::pipeline pipeline;

    rs2::config config;
    config.enable_stream(RS2_STREAM_COLOR, -1, IMAGE_WIDTH, IMAGE_HEIGHT, RS2_FORMAT_YUYV);

    std::cout << "Starting video pipeline..." << std::endl;
    pipeline.start(config);
    std::cout << "Pipeline started." << std::endl;

    while(true)
    {
        rs2::frameset frames = pipeline.wait_for_frames();

        rs2::video_frame image = frames.get_color_frame();

        if(image)
        {
            if( image.get_width() != IMAGE_WIDTH || image.get_height() != IMAGE_HEIGHT )
            {
                std::cerr << "Internal error!" << std::endl;
                exit(1);
            }

            const char* data = reinterpret_cast<const char*>(image.get_data());

            ImageMessage& msg = port.prepare();

            msg.frameid = image.get_frame_number();
            msg.timestamp = image.get_timestamp();
            msg.width = IMAGE_WIDTH;
            msg.height = IMAGE_HEIGHT;
            msg.data.resize(IMAGE_WIDTH*IMAGE_HEIGHT);
            for(int i=0; i<IMAGE_HEIGHT; i++)
            {
                for(int j=0; j<IMAGE_WIDTH; j++)
                {
                    msg.data[IMAGE_WIDTH*i+j] = data[2*i*IMAGE_WIDTH+2*j];
                }
            }

            port.write();
        }
    }

    return 0;
}

