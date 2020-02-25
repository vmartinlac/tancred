#include <iostream>
#include <QPainter>
#include <QPixmap>
#include "VideoWidget.h"

VideoWidget::VideoWidget(LCMThread* conn, QWidget* parent) : QWidget(parent)
{
    myConn = conn;

    setFocusPolicy(Qt::StrongFocus);
}

VideoWidget::~VideoWidget()
{
    //myConn->unsubscribe(mySubscription);
}

void VideoWidget::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);
    painter.setBackground(QBrush(Qt::black));
    //painter.setBackground(QBrush(QPixmap(":/images/background.png")));
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

