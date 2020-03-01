
#pragma once

#include <iostream>
#include <memory>
#include <QImage>
#include <QThread>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <ImageMessage.h>

class LCMInterface : public QThread
{
    Q_OBJECT

public:

    LCMInterface();

    void sendEnableSelfDriving();
    void sendDisableSelfDriving();
    void sendMotorCommand(bool fullstop, double steering, double speed);

signals:

    void imageReceived(int frameid, double timestamp, QImage image);

protected:

    void run() override;

protected:

    std::unique_ptr<yarp::os::Network> myConn;
};

class ImagePort : public QObject, public yarp::os::BufferedPort<ImageMessage>
{
Q_OBJECT

public:

    ImagePort(QObject* parent=nullptr) : QObject(parent)
    {
    }

    void onRead(ImageMessage& msg) override
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

        imageReceived(frameid, timestamp, image);
    }

signals:

    void imageReceived(int frameid, double timestamp, QImage image);

protected:

    LCMInterface* myConn;
};
