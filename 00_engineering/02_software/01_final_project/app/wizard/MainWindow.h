
#pragma once

#include <QLabel>
#include <QMainWindow>
#include "RobotInterface.h"
#include "VideoWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(RobotInterface* conn, QWidget* w=nullptr);

    ~MainWindow() override = default;

protected slots:

    void about();
    void sendEnableAutoPilot();
    void sendDisableAutoPilot();

protected:

    RobotInterface* myConn;
    VideoWidget* myVideo;
};

