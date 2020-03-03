
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include "MotorsMessage.h"

bool MotorsMessage::read(yarp::os::ConnectionReader& reader)
{
    stop = static_cast<bool>( reader.expectInt8() );
    steering = reader.expectDouble();
    speed = reader.expectDouble();
    sequence_number = reader.expectInt32();
    time_to_live = reader.expectInt32();

    return true;
}

bool MotorsMessage::write(yarp::os::ConnectionWriter& writer) const
{
    writer.appendInt8( static_cast<int8_t>(stop) );
    writer.appendDouble(steering);
    writer.appendDouble(speed);
    writer.appendInt32(sequence_number);
    writer.appendInt32(time_to_live);

    return true;
}

