#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <lcm/lcm-cpp.hpp>
#include <ImageMessage.hpp>
#include "common.h"

int main(int num_args, char** args)
{
    using Clock = std::chrono::steady_clock;

    const int width = 640;
    const int height = 480;
    const std::chrono::milliseconds period(40);
    const double tau = 2.0;
    const double lambda = 150.0;

    lcm::LCM conn(LCM_PROVIDER);

    if(conn.good() == false)
    {
        std::cout << "Could not connect to LCM!" << std::endl;
        exit(1);
    }

    int count = 0;

    Clock::time_point t0 = Clock::now();

    while(true)
    {
        std::this_thread::sleep_for(period);

        const double timestamp = (std::chrono::duration_cast<std::chrono::milliseconds>( Clock::now() - t0 ).count()) * 1.0e-3;

        ImageMessage msg;

        msg.frame_id = count++;
        msg.timestamp = timestamp;
        msg.width = width;
        msg.height = height;
        msg.encoding = 0;
        msg.buffer_size = width*height;
        msg.buffer.resize(msg.buffer_size);
        for(int i=0; i<height; i++)
        {
            for(int j=0; j<width; j++)
            {
                uint8_t& pixel = reinterpret_cast<uint8_t&>(msg.buffer[width*i+j]);
                const double s = i + j;
                pixel = static_cast<uint8_t>( 128.0 + 80.0*cos(2.0*M_PI*(timestamp/tau + s/lambda )));
            }
        }

        conn.publish("Camera", &msg);
    }

    return 0;
}

