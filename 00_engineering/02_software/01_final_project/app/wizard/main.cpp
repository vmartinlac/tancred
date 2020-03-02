#include <iostream>
#include <QApplication>
#include "RobotInterface.h"
#include "MainWindow.h"

int main(int num_args, char** args)
{
    QApplication app(num_args, args);
    app.setOrganizationName("vmartinlac");
    app.setApplicationName("tancred_wizard");

    RobotInterface* robot_interface = new RobotInterface();

    MainWindow* w = new MainWindow(robot_interface);
    w->show();

    const int ret = app.exec();

    delete w;
    delete robot_interface;

    return ret;
}

