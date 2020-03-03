#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include "AutopilotMessage.h"

bool AutopilotMessage::read(yarp::os::ConnectionReader& reader)
{
    enable = static_cast<bool>(reader.expectInt8());

    return true;
}

bool AutopilotMessage::write(yarp::os::ConnectionWriter& writer) const
{
    writer.appendInt8(enable);

    return true;
}

