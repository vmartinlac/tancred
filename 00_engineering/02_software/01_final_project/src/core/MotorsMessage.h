
#pragma once

#include <vector>
#include <cstdint>
#include <yarp/os/Portable.h>

class MotorsMessage : public yarp::os::Portable
{
public:

    bool forward;
    double steering;

public:

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;
};

