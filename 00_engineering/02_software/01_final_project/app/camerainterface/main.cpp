#include <chrono>
#include <cstring>
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
        myMinKeyFramePeriod = 30;
        myPreKeyFrame = 0;

        myHasFirstFrameNumber = false;
        myFirstFrameNumber = 0;

        vpx_img_alloc(&myImage, VPX_IMG_FMT_I420, IMAGE_WIDTH, IMAGE_HEIGHT, 1);

        myInterface = vpx_codec_vp9_cx();

        vpx_codec_enc_config_default( myInterface, &myConfig, 0);

        myConfig.g_w = IMAGE_WIDTH;
        myConfig.g_h = IMAGE_HEIGHT;
        myConfig.g_error_resilient = 1;
        myConfig.g_timebase.num = 1;
        myConfig.g_timebase.den = IMAGE_FPS;

        vpx_codec_enc_init(&myContext, myInterface, &myConfig, 0);
    }

    void send(rs2::video_frame frame, yarp::os::BufferedPort<ImageMessage>& port)
    {
        // determine flags.

        vpx_enc_frame_flags_t flags = 0;

        if(myPreKeyFrame > 0)
        {
            myPreKeyFrame--;
        }
        else
        {
            myPreKeyFrame = myMinKeyFramePeriod-1;
            flags |= VPX_EFLAG_FORCE_KF;
        }

        // determine presentation timestamp.

        if(myHasFirstFrameNumber == false)
        {
            myFirstFrameNumber = frame.get_frame_number();
            myHasFirstFrameNumber = true;
        }

        const vpx_codec_pts_t pts = frame.get_frame_number() - myFirstFrameNumber;

        // Copy frame data to vpx image.

        {
            /*
            std::cout << myImage.d_w << ' ' << myImage.d_h << std::endl;
            std::cout << myImage.x_chroma_shift << ' ' << myImage.y_chroma_shift << std::endl;
            std::cout << myImage.stride[0] << ' ' << myImage.stride[1] << ' ' << myImage.stride[2] << std::endl;
            std::cout << std::endl;
            */

            // copy Y.
            for(int i=0; i<IMAGE_HEIGHT; i++)
            {
                const uint8_t* pfrom = static_cast<const uint8_t*>( frame.get_data() ) + i*frame.get_stride_in_bytes();
                uint8_t* pto = myImage.planes[VPX_PLANE_Y] + i*myImage.stride[VPX_PLANE_Y];

                for(int j=0; j<IMAGE_WIDTH; j++)
                {
                    *pto = *pfrom;
                    pfrom += 2;
                    pto++;
                }
            }

            // copy U and V.
            for(int i=0; i<IMAGE_HEIGHT/2; i++)
            {
                const uint8_t* p0 = static_cast<const uint8_t*>( frame.get_data() ) + (2*i+0) * frame.get_stride_in_bytes() + 1;
                const uint8_t* p1 = static_cast<const uint8_t*>( frame.get_data() ) + (2*i+1) * frame.get_stride_in_bytes() + 1;
                uint8_t* pu = myImage.planes[VPX_PLANE_U] + i*myImage.stride[VPX_PLANE_U];
                uint8_t* pv = myImage.planes[VPX_PLANE_V] + i*myImage.stride[VPX_PLANE_V];

                for(int j=0; j<IMAGE_WIDTH/2; j++)
                {
                    *pu = static_cast<uint8_t>( 0.5f * ( float(*p0) + float(*p1) ) );
                    pu++;
                    p0 += 2;
                    p1 += 2;

                    *pv = static_cast<uint8_t>( 0.5f * ( float(*p0) + float(*p1) ) );
                    pv++;
                    p0 += 2;
                    p1 += 2;
                }
            }
        }

        // encode frame.

        vpx_codec_err_t error = vpx_codec_encode(&myContext, &myImage, pts, 1, flags, VPX_DL_REALTIME);
        if(error != VPX_CODEC_OK)
        {
            std::cout << "Error while encoding frame!" << std::endl;
        }

        // send messages.

        vpx_codec_iter_t iter = nullptr;
        const vpx_codec_cx_pkt_t* pkt = nullptr;
        while( (pkt = vpx_codec_get_cx_data(&myContext, &iter)) != nullptr )
        {
            if( pkt->kind == VPX_CODEC_CX_FRAME_PKT )
            {
                ImageMessage& msg = port.prepare();
                msg.frameid = frame.get_frame_number();
                msg.timestamp = frame.get_timestamp();
                msg.width = IMAGE_WIDTH;
                msg.height = IMAGE_HEIGHT;
                msg.format = ImageMessage::FORMAT_VP9;
                msg.data.resize(pkt->data.frame.sz);
                memcpy(msg.data.data(), pkt->data.frame.buf, pkt->data.frame.sz);
                port.write();
            }
        }
    }

    void finalize()
    {
        vpx_img_free(&myImage);
    }

protected:

    bool myHasFirstFrameNumber;
    unsigned long long myFirstFrameNumber;

    int myMinKeyFramePeriod;
    int myPreKeyFrame;

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
    msg.format = ImageMessage::FORMAT_GRAYSCALE8;
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
                if( video_frame.get_width() != IMAGE_WIDTH || video_frame.get_height() != IMAGE_HEIGHT || video_frame.get_profile().format() != RS2_FORMAT_YUYV )
                {
                    std::cerr << "Internal error!" << std::endl;
                    exit(1);
                }

                send_raw_image(video_frame, raw_port);
                codec.send(video_frame, compressed_port);
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

