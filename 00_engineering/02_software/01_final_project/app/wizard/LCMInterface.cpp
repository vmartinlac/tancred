#include <iostream>
#include <SelfDrivingMessage.hpp>
#include <ImageMessage.hpp>
#include <MotorCommandMessage.hpp>
#include "LCMInterface.h"
#include "common.h"

LCMInterface::LCMInterface()
{
    myLCM.reset(new lcm::LCM(LCM_PROVIDER));
}

void LCMInterface::run()
{
    if(myLCM->good() == false)
    {
        std::cout << "Could not initialize LCM!" << std::endl;
        exit(1);
    }

    myLCM->subscribe("Camera", &LCMInterface::onFrameReceived, this);

    while(isInterruptionRequested() == false)
    {
        myLCM->handleTimeout(200);
    }
}

void LCMInterface::onFrameReceived(const lcm::ReceiveBuffer*, const std::string&, const ImageMessage* frame)
{
    QImage image = QImage(
        reinterpret_cast<const uint8_t*>( frame->buffer.data() ),
        frame->width,
        frame->height,
        frame->width,
        QImage::Format_Grayscale8).copy();

    imageReceived(frame->frame_id, frame->timestamp, image);
}

void LCMInterface::sendEnableSelfDriving()
{
    SelfDrivingMessage msg;
    msg.enable = true;
    myLCM->publish("SelfDriving", &msg);
}

void LCMInterface::sendDisableSelfDriving()
{
    SelfDrivingMessage msg;
    msg.enable = false;
    myLCM->publish("SelfDriving", &msg);
}

void LCMInterface::sendMotorCommand(bool fullstop, double steering, double speed)
{
    MotorCommandMessage msg;

    msg.fullstop = fullstop;
    msg.steering = steering;
    msg.speed = speed;

    myLCM->publish("Motors", &msg);
}

