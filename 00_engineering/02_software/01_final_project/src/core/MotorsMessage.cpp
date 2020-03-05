
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include "MotorsMessage.h"

bool MotorsMessage::read(yarp::os::ConnectionReader& reader)
{
    forward = static_cast<bool>( reader.expectInt8() );
    steering = reader.expectDouble();

    return true;
}

bool MotorsMessage::write(yarp::os::ConnectionWriter& writer) const
{
    writer.appendInt8( static_cast<int8_t>(forward) );
    writer.appendDouble(steering);

    return true;
}

