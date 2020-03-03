#include <iostream>
#include "RobotInterface.h"

RobotInterface::ImageCallback::ImageCallback(RobotInterface* interface)
{
    myInterface = interface;
}

void RobotInterface::ImageCallback::onRead(ImageMessage& msg)
{
    const int frameid = msg.frameid;
    const double timestamp = msg.timestamp;
    const int width = msg.width;
    const int height = msg.height;

    QImage image(width, height, QImage::Format_Grayscale8);
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            image.bits()[i*width+j] = msg.data[i*width+j];
        }
    }

    myInterface->imageReceived(frameid, timestamp, image);
}

RobotInterface::RobotInterface() :
    myImageCallback(this)
{

    myMotorsPort.open("/wizard/motors_output");
    myAutopilotPort.open("/wizard/autopilot_output");
    myImagePort.open("/wizard/image_input");
    myImagePort.useCallback(myImageCallback);

    myConn.connect("/camera", myImagePort.getName());
}

RobotInterface::~RobotInterface()
{
    myAutopilotPort.close();
    myImagePort.close();
    myMotorsPort.close();
}

void RobotInterface::sendEnableAutoPilot()
{
    AutopilotMessage& msg = myAutopilotPort.prepare();
    msg.enable = true;
    myAutopilotPort.write();
}

void RobotInterface::sendDisableAutoPilot()
{
    AutopilotMessage& msg = myAutopilotPort.prepare();
    msg.enable = false;
    myAutopilotPort.write();
}

void RobotInterface::sendMotorCommand(bool stop, double steering, double speed, int sequence_number)
{
    MotorsMessage& msg = myMotorsPort.prepare();

    msg.stop = stop;
    msg.steering = steering;
    msg.speed = speed;
    msg.sequence_number = sequence_number;

    myMotorsPort.write();
}

