
#pragma once

#include <iostream>
#include <memory>
#include <QImage>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <ImageMessage.h>
#include <MotorsMessage.h>

class RobotInterface : public QObject
{
    Q_OBJECT

public:

    RobotInterface();
    ~RobotInterface();

    void sendEnableAutoPilot();
    void sendDisableAutoPilot();
    void sendMotorCommand(bool fullstop, double steering, double speed);

signals:

    void imageReceived(int frameid, double timestamp, QImage image);

protected:

    class ImageCallback : public yarp::os::TypedReaderCallback<ImageMessage>
    {
    public:

        ImageCallback(RobotInterface* interface);

        void onRead(ImageMessage& msg) override;

    protected:

        RobotInterface* myInterface;
    };

protected:

    yarp::os::Network myConn;
    yarp::os::BufferedPort<ImageMessage> myImagePort;
    yarp::os::BufferedPort<MotorsMessage> myMotorsPort;
    ImageCallback myImageCallback;
};

