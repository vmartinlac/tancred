
#pragma once

#include <QLabel>
#include <QMainWindow>
#include "LCMThread.h"
#include "VideoWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(LCMThread* conn, QWidget* w=nullptr);

    ~MainWindow() override = default;

protected slots:

    void about();

protected:

    LCMThread* myConn;
    VideoWidget* myVideo;
};

