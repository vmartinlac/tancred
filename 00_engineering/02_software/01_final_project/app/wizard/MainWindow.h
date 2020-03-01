
#pragma once

#include <QLabel>
#include <QMainWindow>
#include "LCMInterface.h"
#include "VideoWidget.h"
#include "ControllerWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(LCMInterface* conn, QWidget* w=nullptr);

    ~MainWindow() override = default;

protected slots:

    void about();
    void sendEnableSelfDriving();
    void sendDisableSelfDriving();

protected:

    LCMInterface* myConn;
    VideoWidget* myVideo;
    ControllerWidget* myController;
};

