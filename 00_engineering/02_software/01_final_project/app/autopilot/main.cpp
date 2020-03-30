#include <iostream>
#include <condition_variable>
#include <future>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <torch/torch.h>
#include <torch/script.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <ImageMessage.h>
#include <AutopilotMessage.h>
#include <MotorsMessage.h>

struct Common
{
    yarp::os::BufferedPort<AutopilotMessage> port_input_autopilot;
    yarp::os::BufferedPort<ImageMessage> port_input_image;
    yarp::os::BufferedPort<MotorsMessage> port_output_motors;

    torch::jit::script::Module model;

    std::mutex mutex_condition;
    std::condition_variable condition;

    volatile bool interruption_requested;
    std::mutex mutex_last_image;
    cv::Mat1b last_image;
    volatile bool forward;
};

class InputImageCallback : public yarp::os::TypedReaderCallback<ImageMessage>
{
public:

    InputImageCallback(Common* c)
    {
        myCommon = c;
    }

    void onRead(ImageMessage& msg) override
    {
        cv::Mat1b image = cv::Mat1b(msg.height, msg.width, reinterpret_cast<unsigned char*>(msg.data.data())).clone();

        myCommon->mutex_last_image.lock();
        myCommon->last_image = image;
        myCommon->mutex_last_image.unlock();

        myCommon->condition.notify_one();
    }

protected:

    Common* myCommon;
};

class InputAutopilotCallback : public yarp::os::TypedReaderCallback<AutopilotMessage>
{
public:

    InputAutopilotCallback(Common* c)
    {
        myCommon = c;
    }

    void onRead(AutopilotMessage& msg) override
    {
        myCommon->forward = msg.enable;

        myCommon->condition.notify_one();
    }

protected:

    Common* myCommon;
};

void driving_procedure(Common* common)
{
    double current_steering = 0.0;
    bool forward = false;

    bool go_on = true;

    torch::Device device(torch::kCUDA);

    while(go_on)
    {
        go_on = !common->interruption_requested;

        if(go_on)
        {
            std::unique_lock<std::mutex> lock(common->mutex_condition);
            common->condition.wait_for(lock, std::chrono::milliseconds(100));

            common->mutex_last_image.lock();
            cv::Mat1b image0 = common->last_image;
            common->last_image = cv::Mat1b();
            common->mutex_last_image.unlock();

            forward = common->forward;

            if(forward && image0.data)
            {
                if(image0.isContinuous() == false)
                {
                    std::cout << "Internal error!" << std::endl;
                    exit(1);
                }

                cv::Mat3b image1;
                cv::cvtColor(image0, image1, cv::COLOR_GRAY2RGB);

                cv::Mat3f image2;
                image1.convertTo(image2, CV_32FC3);

                {
                    const cv::Vec3f mu(0.485f, 0.456f, 0.406f);
                    const cv::Vec3f sigma(0.229f, 0.224f, 0.225f);

                    for(int i=0; i<image2.rows; i++)
                    {
                        float* ptr = reinterpret_cast<float*>( image2.ptr(i) );

                        for(int j=0; j<image2.cols; j++)
                        {
                            ptr[0] = (ptr[0] / 255.0f - mu[0]) / sigma[0];
                            ptr[1] = (ptr[1] / 255.0f - mu[1]) / sigma[1];
                            ptr[2] = (ptr[2] / 255.0f - mu[2]) / sigma[2];

                            /*
                            ptr[0] = (ptr[0] - mu[0]) / sigma[0];
                            ptr[1] = (ptr[1] - mu[1]) / sigma[1];
                            ptr[2] = (ptr[2] - mu[2]) / sigma[2];
                            */

                            ptr += 3;
                        }
                    }
                }

                torch::Tensor input = torch::from_blob(image2.data, {1, image2.rows, image2.cols, 3}, torch::kF32);

                input = input.permute({0, 3, 1, 2});

                input = input.to(device);

                std::vector<torch::jit::IValue> inputs;
                inputs.push_back(input);

                at::Tensor output = common->model.forward(inputs).toTensor();

                if( output.dim() != 2 || output.size(0) != 1 || output.size(1) != 1 )
                {
                    std::cout << "Internal error!" << std::endl;
                    exit(1);
                }

                current_steering = output[0][0].item<float>();
                current_steering = std::max<double>(current_steering, -M_PI/3.0);
                current_steering = std::min<double>(current_steering, M_PI/3.0);
            }

            MotorsMessage& msg = common->port_output_motors.prepare();
            msg.forward = forward;
            msg.steering = current_steering;
            common->port_output_motors.write();

            std::cout << "Sent " << forward << " " << current_steering << std::endl;
        }
    }
}

int main(int num_args, char** args)
{
    if(num_args != 2)
    {
        std::cout << "Bad command line!" << std::endl;
        exit(1);
    }

    const std::string model_path = args[1];

    yarp::os::Network network;

    Common common;
    common.forward = false;
    common.interruption_requested = false;
    common.model = torch::jit::load(model_path);
    common.port_input_autopilot.open("/autopilot/input/autopilot");
    common.port_input_image.open("/autopilot/input/image");
    common.port_output_motors.open("/autopilot/output/motors");

    InputImageCallback image_callback(&common);
    InputAutopilotCallback autopilot_callback(&common);

    common.port_input_autopilot.useCallback(autopilot_callback);
    common.port_input_image.useCallback(image_callback);

    std::thread thread(driving_procedure, &common);

    std::cout << "Press ENTER to quit." << std::endl;
    getchar();

    common.interruption_requested = true;

    common.port_input_autopilot.close();
    common.port_input_image.close();
    common.port_output_motors.close();

    thread.join();

    return 0;
}

