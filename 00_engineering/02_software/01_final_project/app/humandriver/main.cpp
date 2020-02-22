#include <iostream>
#include <QApplication>
#include <lcm/lcm-cpp.hpp>
#include "ConfigDialog.h"
#include "MainWindow.h"

int main(int num_args, char** args)
{
    int ret = 0;

    lcm::LCM conn;

    QApplication app(num_args, args);
    app.setOrganizationName("vmartinlac");
    app.setApplicationName("tancred_humandriver");

    ConfigPtr cfg = ConfigDialog::askConfig();

    if(cfg)
    {
        MainWindow* w = new MainWindow(&conn);
        w->show();
        ret = app.exec();
        delete w;
    }

    return ret;
}

