
#pragma once

#include <QSlider>
#include "LCMInterface.h"

class ControllerWidget : public QSlider
{
    Q_OBJECT

public:

    ControllerWidget(LCMInterface* conn, QWidget* parent=nullptr);
    ~ControllerWidget();

protected slots:

    void onMoved();
    void onPressed();
    void onReleased();

protected:

    LCMInterface* myConn;
};

