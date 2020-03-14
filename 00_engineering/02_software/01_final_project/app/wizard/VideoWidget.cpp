#include <iostream>
#include <cmath>
#include <QPainter>
#include <QPixmap>
#include <constants.h>
#include "VideoWidget.h"

VideoWidget::VideoWidget(RobotInterface* conn, QWidget* parent) : QWidget(parent)
{
    myZoom = 1.0;
    myConn = conn;
    myMode = MODE_SILENT;
    myHasNewImage = false;
    myImageTimer = new QTimer(this);
    myMotorsTimer = new QTimer(this);
    connect(myImageTimer, SIGNAL(timeout()), this, SLOT(refresh()));
    connect(myMotorsTimer, SIGNAL(timeout()), this, SLOT(sendMotorsCommand()));

    myCommandForward = false;
    myCommandSteering = 0.0;

    setFocusPolicy(Qt::StrongFocus);

    myImageTimer->start(1000/30);
    myMotorsTimer->start(MOTOR_COMMAND_TTL_MS/2);

    connect(
        myConn,
        SIGNAL(imageReceived(int,double,QImage)),
        this,
        SLOT(onImageReceived(int,double,QImage)),
        Qt::QueuedConnection);
}

void VideoWidget::sendMotorsCommand()
{
    if(myMode == MODE_DRIVING)
    {
        myConn->sendMotorCommand(myCommandForward, myCommandSteering);
    }
}

void VideoWidget::onImageReceived(int frameid, double timestamp, QImage image)
{
    myHasNewImage = true;
    myImage = image;
}

void VideoWidget::refresh()
{
    const bool do_update = myHasNewImage;
    myHasNewImage = false;

    if(do_update)
    {
        update();
    }
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::wheelEvent(QWheelEvent* ev)
{
    const double factor_max = 20.0;
    const double factor_min = 1.0/20.0;
    const double speed = 2.0e-3;
    const QPoint delta = ev->angleDelta();

    if(delta.isNull() == false)
    {
        const double new_factor = myZoom * std::exp(speed * delta.y());
        myZoom = std::max(factor_min, std::min(factor_max, new_factor));
        update();
    }
}

void VideoWidget::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);

    {
        painter.setBackground(QBrush(Qt::black));
        //painter.setBackground(QBrush(QPixmap(":/images/background.png")));
        painter.eraseRect(painter.viewport());
    }

    const int margin = 20;

    if(myImage.isNull() == false && myImage.width() >= 2*margin && myImage.height() >= 2*margin)
    {
        /*
        double scale = 1.0;
        scale = std::min<double>( scale, double(width()-2*margin) / double(myImage.width()) );
        scale = std::min<double>( scale, double(height()-2*margin) / double(myImage.height()) );
        */
        const double scale = myZoom;

        if( std::min<double>(scale*myImage.width(), scale*myImage.height()) > 30.0 )
        {
            const QTransform transform =
                QTransform::fromTranslate(-myImage.width()/2, -myImage.height()/2) *
                QTransform::fromScale(scale, scale) *
                QTransform::fromTranslate(width()/2, height()/2);

            painter.setTransform(transform);
            painter.drawImage(0, 0, myImage);
            painter.resetTransform();
        }
    }

    {
        const char* str = "";

        switch( myMode )
        {
        case MODE_SILENT:
            str = "SILENT";
            break;
        case MODE_DRIVING:
            str = "DRIVING";
            break;
        }

        const int margin = 10;
        painter.setPen(Qt::white);
        painter.drawText( QPoint(margin, painter.fontMetrics().height()+margin), str );
    }

    {
        const double l = 0.2*std::min<int>(width(), height());
        const int border = 0;
        const int radius = 8;

        QColor col( (myCommandForward) ? Qt::green : Qt::yellow );

        QPen p;
        p.setColor(col);
        p.setWidth(3.0);
        painter.setPen(p);
        painter.drawLine(
            QPoint(width()/2, height() - border),
            QPoint(width()/2+l*sin(myCommandSteering), height() - border - l*cos(myCommandSteering)));
        painter.setBrush(col);
        painter.drawEllipse(QPoint(width()/2, height()-border), radius, radius);
    }
}

double VideoWidget::convertLocationToSteering(const QPoint& pt)
{
    const double dx = pt.x() - width()/2;
    const double dy = height()/2;
    return std::atan(dx/dy);;
}

void VideoWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if(myMode == MODE_DRIVING)
    {
        myCommandForward = true;
        myCommandSteering = convertLocationToSteering(ev->pos());

        sendMotorsCommand();
        update();
    }
}

void VideoWidget::mousePressEvent(QMouseEvent* ev)
{
    if(myMode == MODE_DRIVING)
    {
        myCommandForward = true;
        myCommandSteering = convertLocationToSteering(ev->pos());

        sendMotorsCommand();
        update();
    }
}

void VideoWidget::mouseReleaseEvent(QMouseEvent*)
{
    if(myMode == MODE_DRIVING)
    {
        myCommandForward = false;
        //myCommandSteering = 0.0;

        sendMotorsCommand();
        update();
    }
}

void VideoWidget::setModeToSilent()
{
    myMode = MODE_SILENT;
    update();
}

void VideoWidget::setModeToDriving()
{
    myMode = MODE_DRIVING;
    update();
}

