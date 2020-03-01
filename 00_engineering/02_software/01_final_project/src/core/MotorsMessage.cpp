
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include "MotorsMessage.h"

bool MotorsMessage::read(yarp::os::ConnectionReader& reader)
{
    stop = static_cast<bool>( reader.expectInt8() );
    steering = reader.expectDouble();
    speed = reader.expectDouble();

    return true;
}

bool MotorsMessage::write(yarp::os::ConnectionWriter& writer) const
{
    writer.appendInt8( static_cast<int8_t>(stop) );
    writer.appendDouble(steering);
    writer.appendDouble(speed);

    return true;
}

