#include <iostream>
#include <QApplication>
#include "LCMInterface.h"
#include "MainWindow.h"

int main(int num_args, char** args)
{
    QApplication app(num_args, args);
    app.setOrganizationName("vmartinlac");
    app.setApplicationName("tancred_wizard");

    LCMInterface* lcm_thread = new LCMInterface();
    lcm_thread->start();

    MainWindow* w = new MainWindow(lcm_thread);
    w->show();
    const int ret = app.exec();
    delete w;

    lcm_thread->exit();
    lcm_thread->wait();
    delete lcm_thread;

    return ret;
}

