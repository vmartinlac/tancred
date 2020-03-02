
#pragma once

#include <QTimer>
#include <QMutex>
#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include "RobotInterface.h"

class VideoWidget : public QWidget
{
    Q_OBJECT

public:

    enum Mode
    {
        MODE_SILENT,
        MODE_DRIVING,
        MODE_RECORDING
    };

public:

    VideoWidget(RobotInterface* video, QWidget* parent=nullptr);
    ~VideoWidget();

public slots:

    void refresh();
    void setModeToSilent();
    void setModeToDriving();
    void setModeToRecording();
    void onImageReceived(int frameid, double timestamp, QImage image);

protected:

    void paintEvent(QPaintEvent* ev);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDownEvent(QMouseEvent*);
    void mouseUpEvent(QMouseEvent*);

protected:

    RobotInterface* myConn;
    Mode myMode;
    bool myHasNewImage;
    QImage myImage;
    QTimer* myTimer;
};

