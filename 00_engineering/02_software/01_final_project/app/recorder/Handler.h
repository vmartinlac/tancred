
#pragma once

#include <QDir>
#include <QObject>
#include <QTimer>
#include <sqlite3.h>
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

    void init(const QString& output_directory);
    void finalize();

signals:

    void imageMessageReceived(InternalImageMessagePtr);
    void motorsMessageReceived(InternalMotorsMessagePtr);

protected slots:

    void receiveImageMessage(InternalImageMessagePtr msg);
    void receiveMotorsMessage(InternalMotorsMessagePtr msg);
    void checkHearthBeat();

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
    ClockType::time_point myClockReference;

    QDir myDirectory;
    sqlite3* myDatabase;
    sqlite3_stmt* myImageStatement;
    sqlite3_stmt* myMotorsStatement;

    std::atomic<int> myNumImageMessagesToProcess;
    std::atomic<int> myNumMotorsMessagesToProcess;
    std::atomic<int> myNumSkippedImageMessages;
    std::atomic<int> myNumSkippedMotorsMessages;

    int myNextImageId;

    QTimer* myHeartbeatTimer;
    ClockType::time_point myTimeLast;
};

