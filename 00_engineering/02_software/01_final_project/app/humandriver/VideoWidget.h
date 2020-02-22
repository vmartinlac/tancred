
#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <lcm/lcm-cpp.hpp>

class VideoWidget : public QWidget
{
public:

    VideoWidget(lcm::LCM* video, QWidget* parent=nullptr);

protected:

    void paintEvent(QPaintEvent* ev);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDownEvent(QMouseEvent*);
    void mouseUpEvent(QMouseEvent*);

protected:

    lcm::LCM* myConn;
};

