
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
        MODE_DRIVING
    };

public:

    VideoWidget(RobotInterface* video, QWidget* parent=nullptr);
    ~VideoWidget();

public slots:

    void refresh();
    void setModeToSilent();
    void setModeToDriving();
    void onImageReceived(int frameid, double timestamp, QImage image);
    void sendMotorsCommand();

protected:

    void paintEvent(QPaintEvent* ev) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    double convertLocationToSteering(const QPoint& pt);

protected:

    RobotInterface* myConn;
    Mode myMode;
    bool myHasNewImage;
    QImage myImage;
    QTimer* myImageTimer;
    QTimer* myMotorsTimer;
    bool myCommandForward;
    double myCommandSteering;
};

