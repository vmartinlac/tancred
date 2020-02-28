#include <iostream>
#include <SelfDrivingMessage.hpp>
#include <ImageMessage.hpp>
#include <MotorCommandMessage.hpp>
#include "LCMThread.h"
#include "common.h"

void LCMThread::run()
{
    myLCM = new lcm::LCM(LCM_PROVIDER);

    if(myLCM->good() == false)
    {
        std::cout << "Could not initialize LCM!" << std::endl;
        exit(1);
    }

    myLCM->subscribe("Camera", &LCMThread::onFrameReceived, this);

    mySockerNotifier = new QSocketNotifier(myLCM->getFileno(), QSocketNotifier::Read);
    mySockerNotifier->setEnabled(true);
    connect(mySockerNotifier, SIGNAL(activated(int)), this, SLOT(onSocketActivated()));

    std::cout << "LCMThread started!" << std::endl;

    QThread::run();

    std::cout << "LCMThread finishing..." << std::endl;

    delete mySockerNotifier;
    delete myLCM;
}

void LCMThread::onFrameReceived(const lcm::ReceiveBuffer*, const std::string&, const ImageMessage* frame)
{
    QImage image = QImage(
        reinterpret_cast<const uint8_t*>( frame->buffer.data() ),
        frame->width,
        frame->height,
        frame->width,
        QImage::Format_Grayscale8).copy();

    onImageReceived(
        frame->frame_id,
        frame->timestamp,
        image);
}

void LCMThread::onSocketActivated()
{
    myLCM->handleTimeout(1);
}

void LCMThread::sendEnableSelfDriving()
{
    SelfDrivingMessage msg;
    msg.enable = true;
    myLCM->publish("SelfDriving", &msg);
}

void LCMThread::sendDisableSelfDriving()
{
    SelfDrivingMessage msg;
    msg.enable = false;
    myLCM->publish("SelfDriving", &msg);
}

void LCMThread::sendMotorCommand(bool fullstop, double steering, double speed)
{
    MotorCommandMessage msg;

    msg.fullstop = fullstop;
    msg.steering = steering;
    msg.speed = speed;

    myLCM->publish("Motors", &msg);
}

