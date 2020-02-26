
#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include "LCMThread.h"

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

    VideoWidget(LCMThread* video, QWidget* parent=nullptr);
    ~VideoWidget();

public slots:

    void setModeToSilent();
    void setModeToDriving();
    void setModeToRecording();

protected slots:

    void onImageReceived(int frame, double timestamp, const QImage& image);

protected:

    void paintEvent(QPaintEvent* ev);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDownEvent(QMouseEvent*);
    void mouseUpEvent(QMouseEvent*);

protected:

    LCMThread* myConn;
    QImage myImage;
    Mode myMode;
};

