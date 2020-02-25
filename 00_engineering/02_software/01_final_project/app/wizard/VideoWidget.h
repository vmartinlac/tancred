
#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include "LCMThread.h"

class VideoWidget : public QWidget
{
    Q_OBJECT

public:

    VideoWidget(LCMThread* video, QWidget* parent=nullptr);
    ~VideoWidget();

protected:

    void paintEvent(QPaintEvent* ev);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDownEvent(QMouseEvent*);
    void mouseUpEvent(QMouseEvent*);

protected:

    LCMThread* myConn;
};

