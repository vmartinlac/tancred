#include "ControllerWidget.h"

ControllerWidget::ControllerWidget(LCMInterface* conn, QWidget* parent) : QSlider(parent)
{
    myConn = conn;

    setOrientation(Qt::Horizontal);
    setMinimum(-100);
    setMaximum(100);
    setValue(0);
    setTickPosition(QSlider::TicksBelow);

    connect(this, SIGNAL(sliderPressed()), this, SLOT(onPressed()));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(onReleased()));
    //connect(this, SIGNAL(sliderMoved()), this, SLOT(onMoved()));
}

ControllerWidget::~ControllerWidget()
{
}

void ControllerWidget::onMoved()
{
    // TODO
}

void ControllerWidget::onPressed()
{
    // TODO
}

void ControllerWidget::onReleased()
{
    myConn->sendMotorCommand(true, 0.0, 0.0);
}

