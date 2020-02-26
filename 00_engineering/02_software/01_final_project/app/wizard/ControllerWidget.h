
#pragma once

#include <QSlider>
#include "LCMThread.h"

class ControllerWidget : public QSlider
{
    Q_OBJECT

public:

    ControllerWidget(LCMThread* conn, QWidget* parent=nullptr);
    ~ControllerWidget();

protected slots:

    void onMoved();
    void onPressed();
    void onReleased();

protected:

    LCMThread* myConn;
};

