
#pragma once

#include <QLabel>
#include <QMainWindow>
#include <lcm/lcm-cpp.hpp>
#include "VideoWidget.h"

class MainWindow : public QMainWindow
{
public:

    MainWindow(lcm::LCM* conn, QWidget* w=nullptr);

    ~MainWindow() override = default;

protected:

    lcm::LCM* myConn;
    VideoWidget* myVideo;
};

