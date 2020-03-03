
#pragma once

#include <vector>
#include <cstdint>
#include <yarp/os/Portable.h>

class AutopilotMessage : public yarp::os::Portable
{
public:

    bool enable;

public:

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;
};

