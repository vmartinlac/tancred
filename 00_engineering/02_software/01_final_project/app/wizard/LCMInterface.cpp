#include <iostream>
#include "LCMInterface.h"

LCMInterface::LCMInterface()
{
    myConn.reset(new yarp::os::Network());
}

void LCMInterface::run()
{
    ImagePort port;
    port.useCallback();
    port.open("/wizard/camera_input");

    myConn->connect("/camera", port.getName());

    QObject::connect(&port, SIGNAL(imageReceived(int,double,QImage)), this, SIGNAL(imageReceived(int,double,QImage)));

    QThread::run();
}

void LCMInterface::sendEnableSelfDriving()
{
    // FIXME
    /*
    SelfDrivingMessage msg;
    msg.enable = true;
    myLCM->publish("SelfDriving", &msg);
    */
}

void LCMInterface::sendDisableSelfDriving()
{
    // FIXME
    /*
    SelfDrivingMessage msg;
    msg.enable = false;
    myLCM->publish("SelfDriving", &msg);
    */
}

void LCMInterface::sendMotorCommand(bool fullstop, double steering, double speed)
{
    // FIXME
    /*
    MotorCommandMessage msg;

    msg.fullstop = fullstop;
    msg.steering = steering;
    msg.speed = speed;

    myLCM->publish("Motors", &msg);
    */
}

