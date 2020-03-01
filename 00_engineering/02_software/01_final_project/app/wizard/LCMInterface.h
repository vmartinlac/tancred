
#pragma once

#include <memory>
#include <QImage>
#include <QSocketNotifier>
#include <QThread>
#include <lcm/lcm-cpp.hpp>
#include <ImageMessage.hpp>

class LCMInterface : public QThread
{
    Q_OBJECT

public:

    LCMInterface();

    void sendEnableSelfDriving();
    void sendDisableSelfDriving();
    void sendMotorCommand(bool fullstop, double steering, double speed);

signals:

    void imageReceived(int frameid, double timestamp, QImage image);

protected:

    void run() override;
    void onFrameReceived(const lcm::ReceiveBuffer*, const std::string&, const ImageMessage*);

protected:

    std::unique_ptr<lcm::LCM> myLCM;
};

