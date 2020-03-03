
#pragma once

#include <vector>
#include <cstdint>
#include <yarp/os/Portable.h>

class MotorsMessage : public yarp::os::Portable
{
public:

    bool stop;
    double steering;
    double speed;
    int sequence_number;
    int time_to_live;

public:

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;
};

