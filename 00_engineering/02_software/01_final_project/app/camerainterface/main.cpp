#include <chrono>
#include <iostream>
#include <thread>
#include <librealsense2/rs.hpp>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <vpx/vpx_codec.h>
#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>
#include <ImageMessage.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define IMAGE_FPS 15

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

                if( p.is<rs2::video_stream_profile>() && p.fps() == IMAGE_FPS && p.format() == RS2_FORMAT_YUYV && p.stream_type() == RS2_STREAM_COLOR )
                {
                    rs2::video_stream_profile vp = p.as<rs2::video_stream_profile>();
                    
                    if( vp.width() == IMAGE_WIDTH && vp.height() == IMAGE_HEIGHT )
                    {
                        found = true;
                        mySensor = s;
                        myProfile = p;
                    }
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

class Codec
{
public:

    void init()
    {
        vpx_img_alloc(&myImage, VPX_IMG_FMT_I420, IMAGE_WIDTH, IMAGE_HEIGHT, 1);

        myInterface = vpx_codec_vp9_cx();

        vpx_codec_enc_config_default( myInterface, &myConfig, 0);

        myConfig.g_w = IMAGE_WIDTH;
        myConfig.g_h = IMAGE_HEIGHT;
        myConfig.kf_mode = VPX_KF_FIXED;
        //myConfig.kf_max_dist = IMAGE_FPS;
        //config.g_timebase.numerator = 1;
        //config.g_timebase.denominator = IMAGE_FPS; // TODO: use what realsense2 provides.

        vpx_codec_enc_init(&myContext, myInterface, &myConfig, 0);
    }

    void send(rs2::video_frame frame, yarp::os::BufferedPort<ImageMessage>& port)
    {
        // TODO copy rs2 frame to vpx image.

        vpx_enc_frame_flags_t flags = 0;
        vpx_codec_encode(&myContext, &myImage, myNextPts, 1, flags, VPX_DL_REALTIME);
        myNextPts++;

        vpx_codec_iter_t iter = nullptr;
        const vpx_codec_cx_pkt_t* pkt = nullptr;
        while( (pkt = vpx_codec_get_cx_data(&myContext, &iter)) != nullptr )
        {
            if( pkt->kind == VPX_CODEC_CX_FRAME_PKT )
            {
                // TODO: send packet.
                std::cout << "TODO: send video packet" << std::endl;
            }
        }
    }

    void finalize()
    {
        vpx_img_free(&myImage);
    }

protected:

    vpx_codec_pts_t myNextPts;
    vpx_image_t myImage;
    vpx_codec_iface_t* myInterface;
    vpx_codec_ctx_t myContext;
    vpx_codec_enc_cfg_t myConfig;
};

void send_raw_image(rs2::video_frame image, yarp::os::BufferedPort<ImageMessage>& port)
{
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

int main(int num_args, char** args)
{
    // initialize realsense2.

    VideoInput input;
    input.init();

    // initialize VPX.

    Codec codec;
    codec.init();

    // initialize YARP.

    yarp::os::Network yarp;
    yarp::os::BufferedPort<ImageMessage> raw_port;
    yarp::os::BufferedPort<ImageMessage> compressed_port;

    raw_port.open("/camerainterface/raw_image_output");
    compressed_port.open("/camerainterface/compressed_image_output");

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
                if( video_frame.get_width() != IMAGE_WIDTH || video_frame.get_height() != IMAGE_HEIGHT )
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
    compressed_port.close();
    codec.finalize();
    input.finalize();

    return 0;
}

