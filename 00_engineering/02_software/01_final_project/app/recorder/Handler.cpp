#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <opencv2/imgcodecs.hpp>
#include <QMetaObject>
#include <QMetaMethod>
#include "Handler.h"

namespace SqlCommands
{
    static const char* schema[] =
    {
R"(
CREATE TABLE IF NOT EXISTS motors_messages(
    id INTEGER PRIMARY KEY,
    local_timestamp FLOAT,
    forward INTEGER,
    steering FLOAT)
)",

R"(
CREATE TABLE IF NOT EXISTS image_messages(
    id INTEGER PRIMARY KEY,
    local_timestamp FLOAT,
    frameid INTEGER,
    timestamp FLOAT,
    filename STRING)
)",

        nullptr
    };

    static const char* insert_motors_message = "INSERT INTO motors_messages(local_timestamp,forward,steering) VALUES(?,?,?)";
    static const char* insert_image_message = "INSERT INTO image_messages(local_timestamp,frameid,timestamp,filename) VALUES(?,?,?,?)";
}

Handler::Handler(QObject* parent) :
    QObject(parent),
    myMotorsCallback(this),
    myImageCallback(this),
    myDatabase(nullptr),
    myImageStatement(nullptr),
    myMotorsStatement(nullptr)
{
}

Handler::~Handler()
{
}

void Handler::init(const QString& output_directory)
{
    const char* err = "";
    bool ok = true;

    if(ok)
    {
        myNextImageId = 0;

        myClockReference = ClockType::now();

        myNumImageMessagesToProcess = 0;
        myNumMotorsMessagesToProcess = 0;
        myNumSkippedImageMessages = 0;
        myNumSkippedMotorsMessages = 0;

        myImagePort.open("/recorder/camera_input");
        myMotorsPort.open("/recorder/motors_input");

        myImagePort.useCallback(myImageCallback);
        myMotorsPort.useCallback(myMotorsCallback);

        QObject::connect(
            this,
            SIGNAL(imageMessageReceived(InternalImageMessagePtr)),
            this,
            SLOT(receiveImageMessage(InternalImageMessagePtr)),
            Qt::QueuedConnection);

        QObject::connect(
            this,
            SIGNAL(motorsMessageReceived(InternalMotorsMessagePtr)),
            this,
            SLOT(receiveMotorsMessage(InternalMotorsMessagePtr)),
            Qt::QueuedConnection);
    }


    if(ok)
    {
        myDirectory = QDir();
        ok = myDirectory.cd(output_directory);
        err = "Could not change directory to output directory!";
    }

    if(ok)
    {
        const QString db_path = myDirectory.absoluteFilePath("db.sqlite");

        ok = ( sqlite3_open(db_path.toUtf8().data(), &myDatabase) == SQLITE_OK );
        err = "Could not open db!";
    }

    if(ok)
    {
        for(int i=0; ok && SqlCommands::schema[i] != nullptr; i++)
        {
            sqlite3_stmt* stmt = nullptr;

            ok =
                ( sqlite3_prepare_v2(myDatabase, SqlCommands::schema[i], -1, &stmt, nullptr) == SQLITE_OK ) &&
                ( sqlite3_step(stmt) == SQLITE_DONE ) &&
                ( sqlite3_finalize(stmt) == SQLITE_OK );
        }

        err = "Error while initializing DB!";
    }

    if(ok)
    {
        ok = ( sqlite3_prepare_v2(myDatabase, SqlCommands::insert_image_message, -1, &myImageStatement, nullptr) == SQLITE_OK );
        err = "Could not create insert_image_message statement!";
    }

    if(ok)
    {
        ok = ( sqlite3_prepare_v2(myDatabase, SqlCommands::insert_motors_message, -1, &myMotorsStatement, nullptr) == SQLITE_OK );
        err = "Could not create insert_motors_message statement!";
    }

    if(ok == false)
    {
        std::cout << err << std::endl;
        exit(1);
    }
}

void Handler::finalize()
{
    myImagePort.close();
    myMotorsPort.close();

    sqlite3_finalize(myImageStatement);
    myImageStatement = nullptr;

    sqlite3_finalize(myMotorsStatement);
    myMotorsStatement = nullptr;

    sqlite3_close(myDatabase);
    myDatabase = nullptr;

    std::cout << "Num skipped image messages: " << myNumSkippedImageMessages << std::endl;
    std::cout << "Num skipped motors messages: " << myNumSkippedMotorsMessages << std::endl;
}

void Handler::receiveImageMessage(InternalImageMessagePtr msg)
{
    std::vector<uint8_t> encoded;
    QString image_path;
    QString image_filename;
    bool ok = true;
    const char* err = "";
    int fd = -1;

    // encode image.

    if(ok)
    {
        cv::Mat1b image(msg->height, msg->width, reinterpret_cast<uint8_t*>(msg->data.data()));

        ok = cv::imencode("png", image, encoded);
        err = "Could not encode image!";
    }

    // save image on filesystem.

    if(ok)
    {
        do
        {
            image_filename = QString("IMG_%1").arg( myNextImageId, 5, 10, QChar('0') );
            image_path = myDirectory.absoluteFilePath(image_filename);
            myNextImageId++;
        }
        while(myDirectory.exists(image_filename));
    }

    if(ok)
    {
        fd = open(image_path.toUtf8().data(), O_WRONLY|O_SYNC);
        ok = (fd >= 0);
        err = "Could not open image file for writing!";
    }

    if(ok)
    {
        ok = ( write(fd, encoded.data(), encoded.size()) == encoded.size() );
        err = "Could not write image on filesystem!";
    }

    if(ok)
    {
        close(fd);
        fd = -1;
    }

    // record image in database.

    if(ok)
    {
        const double dt = std::chrono::duration_cast<std::chrono::milliseconds>(msg->local_timestamp - myClockReference).count() * 1.0e-3;

        sqlite3_reset(myImageStatement);

        sqlite3_bind_double(myImageStatement, 1, dt);
        sqlite3_bind_int(myImageStatement, 2, msg->frameid);
        sqlite3_bind_double(myImageStatement, 3, msg->timestamp);
        sqlite3_bind_text(myImageStatement, 4, image_filename.toUtf8().data(), -1, SQLITE_TRANSIENT);

        sqlite3_step(myImageStatement);
    }

    if(ok)
    {
        myNumImageMessagesToProcess--;
    }

    if( fd >= 0 )
    {
        close(fd);
    }

    if(ok == false)
    {
        std::cout << err << std::endl;
        exit(1);
    }
}

void Handler::receiveMotorsMessage(InternalMotorsMessagePtr msg)
{
    const double dt = std::chrono::duration_cast<std::chrono::milliseconds>(msg->local_timestamp - myClockReference).count() * 1.0e-3;

    sqlite3_reset(myMotorsStatement);

    sqlite3_bind_double(myMotorsStatement, 1, dt);
    sqlite3_bind_int(myMotorsStatement, 2, msg->forward);
    sqlite3_bind_double(myMotorsStatement, 3, msg->steering);

    sqlite3_step(myMotorsStatement);

    myNumMotorsMessagesToProcess--;
}

Handler::ImageCallback::ImageCallback(Handler* h)
{
    myHandler = h;
}

void Handler::ImageCallback::onRead(ImageMessage& msg)
{
    if(myHandler->myNumImageMessagesToProcess >= 1)
    {
        myHandler->myNumSkippedImageMessages++;
    }
    else
    {
        myHandler->myNumImageMessagesToProcess++;

        InternalImageMessagePtr new_msg(new InternalImageMessage);

        new_msg->local_timestamp = ClockType::now();
        new_msg->frameid = msg.frameid;
        new_msg->timestamp = msg.timestamp;
        new_msg->width = msg.width;
        new_msg->height = msg.height;
        new_msg->data.swap(msg.data);

        myHandler->imageMessageReceived(new_msg);
    }
}

Handler::MotorsCallback::MotorsCallback(Handler* h)
{
    myHandler = h;
}

void Handler::MotorsCallback::onRead(MotorsMessage& msg)
{
    if(myHandler->myNumMotorsMessagesToProcess >= 100)
    {
        myHandler->myNumSkippedMotorsMessages++;
    }
    else
    {
        myHandler->myNumMotorsMessagesToProcess++;

        InternalMotorsMessagePtr new_msg(new InternalMotorsMessage);

        new_msg->local_timestamp = ClockType::now();
        new_msg->forward = msg.forward;
        new_msg->steering = msg.steering;

        myHandler->motorsMessageReceived(new_msg);
    }
}

