#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include "ImageMessage.h"

bool ImageMessage::read(yarp::os::ConnectionReader& reader)
{
    frameid = reader.expectInt32();
    timestamp = reader.expectDouble();
    width = reader.expectInt32();
    height = reader.expectInt32();
    format = reader.expectInt32();
    data.resize(width*height);
    reader.expectBlock(data.data(), width*height);

    return true;
}

bool ImageMessage::write(yarp::os::ConnectionWriter& writer) const
{
    bool ret = false;

    if( width >= 0 && height >= 0 && data.size() == width*height )
    {
        writer.appendInt32(frameid);
        writer.appendDouble(timestamp);
        writer.appendInt32(width);
        writer.appendInt32(height);
        writer.appendInt32(format);
        writer.appendBlock(data.data(), data.size());
        ret = true;
    }

    return ret;
}

