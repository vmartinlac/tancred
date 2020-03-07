#include <QCoreApplication>
#include <QMetaType>
#include <iostream>
#include <yarp/os/Network.h>
#include <ImageMessage.h>
#include <MotorsMessage.h>
#include "Handler.h"

int main(int num_args, char** args)
{
    QCoreApplication app(num_args, args);
    yarp::os::Network yarp;

    qRegisterMetaType<InternalImageMessagePtr>();
    qRegisterMetaType<InternalMotorsMessagePtr>();

    Handler* handler = new Handler();
    handler->init();

    const int ret = app.exec();

    handler->finalize();

    delete handler;

    return ret;
}

