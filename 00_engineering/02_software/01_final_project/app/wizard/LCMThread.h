
#pragma once

#include <memory>
#include <QSocketNotifier>
#include <QThread>
#include <lcm/lcm-cpp.hpp>
#include "ImageMessage.hpp"

class LCMThread : public QThread
{
    Q_OBJECT

public:

    void run() override;

public slots:

    void sendEnableSelfDriving();
    void sendDisableSelfDriving();
    void sendMotorCommand(bool fullstop, double steering, double speed);

protected:

    void onFrameReceived(const lcm::ReceiveBuffer*, const std::string&, const ImageMessage*);

protected slots:

    void onSocketActivated();

protected:

    lcm::LCM* myLCM;
    QSocketNotifier* mySockerNotifier;
};

