
#pragma once

#include <QObject>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/TypedReaderCallback.h>
#include <ImageMessage.h>
#include <MotorsMessage.h>
#include "InternalMessages.h"

class Handler : public QObject
{
    Q_OBJECT

public:

    Handler(QObject* parent=nullptr);
    ~Handler();

    void init();
    void finalize();

signals:

    void imageMessageReceived(InternalImageMessagePtr);
    void motorsMessageReceived(InternalMotorsMessagePtr);

public slots:

    void receiveImageMessage(InternalImageMessagePtr msg);
    void receiveMotorsMessage(InternalMotorsMessagePtr msg);

protected:

    class ImageCallback : public yarp::os::TypedReaderCallback<ImageMessage>
    {
    public:

        ImageCallback(Handler* h);
        ImageCallback(const ImageCallback& o) = delete;

        void onRead(ImageMessage& msg) override;

    protected:

        Handler* myHandler;
    };

    class MotorsCallback : public yarp::os::TypedReaderCallback<MotorsMessage>
    {
    public:

        MotorsCallback(Handler* h);
        MotorsCallback(const MotorsCallback& o) = delete;

        void onRead(MotorsMessage& msg) override;

    protected:

        Handler* myHandler;
    };

protected:

    yarp::os::BufferedPort<ImageMessage> myImagePort;
    yarp::os::BufferedPort<MotorsMessage> myMotorsPort;

    MotorsCallback myMotorsCallback;
    ImageCallback myImageCallback;
};

