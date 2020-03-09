#include <iostream>
#include <thread>
#include <QCoreApplication>
#include <QMetaType>
#include <yarp/os/Network.h>
#include <ImageMessage.h>
#include <MotorsMessage.h>
#include "Handler.h"

void ask_user_to_quit()
{
    std::cout << "Press ENTER to quit" << std::endl;
    getchar();
    QMetaObject::invokeMethod( QCoreApplication::instance(), "quit", Qt::QueuedConnection );
}

int main(int num_args, char** args)
{
    if(num_args != 2)
    {
        std::cout << "Please provide the path to output directory on command line!" << std::endl;
        exit(1);
    }

    QCoreApplication app(num_args, args);
    yarp::os::Network yarp;

    qRegisterMetaType<InternalImageMessagePtr>();
    qRegisterMetaType<InternalMotorsMessagePtr>();

    Handler* handler = new Handler();
    handler->init(args[1]);

    std::thread thread(ask_user_to_quit);

    const int ret = app.exec();

    thread.join();

    handler->finalize();

    delete handler;

    return ret;
}

