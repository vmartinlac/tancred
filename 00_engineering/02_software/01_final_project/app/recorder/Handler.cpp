#include <iostream>
#include <QMetaObject>
#include <QMetaMethod>
#include "Handler.h"

Handler::Handler(QObject* parent) :
    QObject(parent),
    myMotorsCallback(this),
    myImageCallback(this)
{
}

Handler::~Handler()
{
}

void Handler::init()
{
    myImagePort.open("/recorder/camera_input");
    myMotorsPort.open("/recorder/motors_input");

    myImagePort.useCallback(myImageCallback);
    myMotorsPort.useCallback(myMotorsCallback);

    QObject::connect(
        this,
        SIGNAL(imageMessageReceived(InternalImageMessagePtr)),
        this,
        SLOT(receiveImageMessage(InternalImageMessagePtr)),
        Qt::QueuedConnection);

    QObject::connect(
        this,
        SIGNAL(motorsMessageReceived(InternalMotorsMessagePtr)),
        this,
        SLOT(receiveMotorsMessage(InternalMotorsMessagePtr)),
        Qt::QueuedConnection);
}

void Handler::finalize()
{
    myImagePort.close();
    myMotorsPort.close();
}

void Handler::receiveImageMessage(InternalImageMessagePtr msg)
{
    std::cout << "HELLO image" << std::endl;
}

void Handler::receiveMotorsMessage(InternalMotorsMessagePtr msg)
{
    std::cout << "HELLO motors" << std::endl;
}

Handler::ImageCallback::ImageCallback(Handler* h)
{
    myHandler = h;
}

void Handler::ImageCallback::onRead(ImageMessage& msg)
{
    InternalImageMessagePtr new_msg(new InternalImageMessage);

    new_msg->local_timestamp = ClockType::now();
    new_msg->frameid = msg.frameid;
    new_msg->timestamp = msg.timestamp;
    new_msg->width = msg.width;
    new_msg->height = msg.height;
    new_msg->data.swap(msg.data);

    myHandler->imageMessageReceived(new_msg);
}

Handler::MotorsCallback::MotorsCallback(Handler* h)
{
    myHandler = h;
}

void Handler::MotorsCallback::onRead(MotorsMessage& msg)
{
    InternalMotorsMessagePtr new_msg(new InternalMotorsMessage);

    new_msg->local_timestamp = ClockType::now();
    new_msg->forward = msg.forward;
    new_msg->steering = msg.steering;

    myHandler->motorsMessageReceived(new_msg);
}

