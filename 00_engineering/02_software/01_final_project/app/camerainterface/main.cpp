#include <librealsense2/rs.hpp>
#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <ImageMessage.hpp>
#include "common.h"

int main(int num_args, char** args)
{
    lcm::LCM conn(LCM_PROVIDER);

    if(conn.good() == false)
    {
        std::cout << "Could not connect to LCM!" << std::endl;
        exit(1);
    }

    rs2::pipeline pipeline;

    rs2::config config;
    config.enable_stream(RS2_STREAM_COLOR, -1, 640, 480, RS2_FORMAT_RGB8);

    std::cout << "Starting video pipeline..." << std::endl;
    pipeline.start(config);
    std::cout << "Pipeline started." << std::endl;

    while(true)
    {
        rs2::frameset frames = pipeline.wait_for_frames();

        rs2::video_frame image = frames.get_color_frame();

        if(image)
        {
            ImageMessage msg;

            msg.frame_id = image.get_frame_number();
            msg.timestamp = image.get_timestamp();
            msg.width = image.get_width();
            msg.height = image.get_height();
            msg.encoding = 0;
            msg.buffer_size = image.get_width()*image.get_height();
            msg.buffer.resize(msg.buffer_size);
            //memcpy(msg.buffer.data(), image.get_data(), msg.buffer_size);
            for(int i=0; i<480; i++)
            {
                for(int j=0; j<640; j++)
                {
                    msg.buffer[i*640+j] = reinterpret_cast<const uint8_t*>(image.get_data())[640*3*i + 3*j + 0];
                }
            }

            conn.publish("Camera", &msg);
        }
    }

    return 0;
}

