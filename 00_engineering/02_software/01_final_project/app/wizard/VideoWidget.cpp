#include <iostream>
#include <QPainter>
#include <QPixmap>
#include "VideoWidget.h"

VideoWidget::VideoWidget(RobotInterface* conn, QWidget* parent) : QWidget(parent)
{
    myConn = conn;
    myMode = MODE_SILENT;
    myHasNewImage = false;
    myTimer = new QTimer(this);
    connect(myTimer, SIGNAL(timeout()), this, SLOT(refresh()));

    setFocusPolicy(Qt::StrongFocus);

    setMinimumSize(640, 480);

    myTimer->start(1000/30);

    connect(
        myConn,
        SIGNAL(imageReceived(int,double,QImage)),
        this,
        SLOT(onImageReceived(int,double,QImage)),
        Qt::QueuedConnection);
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

void VideoWidget::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);

    {
        painter.setBackground(QBrush(Qt::black));
        //painter.setBackground(QBrush(QPixmap(":/images/background.png")));
        painter.eraseRect(painter.viewport());
    }

    if(myImage.isNull() == false && myImage.width() > 0 && myImage.height() > 0)
    {
        const int margin = 20;

        double scale = 1.0;
        scale = std::min<double>( scale, double(width()-2*margin) / double(myImage.width()) );
        scale = std::min<double>( scale, double(height()-2*margin) / double(myImage.height()) );

        if( std::min<double>(scale*myImage.width(), scale*myImage.height()) > 30.0 )
        {

            const double deltax = width()*0.5 - myImage.width()*scale*0.5;
            const double deltay = height()*0.5 - myImage.height()*scale*0.5;

            QTransform transform =
                QTransform::fromScale(scale, scale) *
                QTransform::fromTranslate(deltax, deltay);

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
        case MODE_RECORDING:
            str = "RECORDING";
            break;
        }

        const int margin = 10;
        painter.setPen(Qt::white);
        painter.drawText( QPoint(margin, painter.fontMetrics().height()+margin), str );
    }
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

void VideoWidget::setModeToRecording()
{
    myMode = MODE_RECORDING;
    update();
}

