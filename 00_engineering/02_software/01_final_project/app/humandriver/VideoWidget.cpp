#include <QPainter>
#include "VideoWidget.h"

VideoWidget::VideoWidget(lcm::LCM* video, QWidget* parent) : QWidget(parent)
{
    myConn = video;

    setFocusPolicy(Qt::StrongFocus);
}

void VideoWidget::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);
    painter.setBackground(QBrush(Qt::black));
    painter.eraseRect(painter.viewport());
}

void VideoWidget::mouseMoveEvent(QMouseEvent*)
{
}

void VideoWidget::mouseDownEvent(QMouseEvent*)
{
}

void VideoWidget::mouseUpEvent(QMouseEvent*)
{
}

