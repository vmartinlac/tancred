
#pragma once

#include <vector>
#include <cstdint>
#include <yarp/os/Portable.h>

class ImageMessage : public yarp::os::Portable
{
public:

    int frameid;
    double timestamp;
    int width;
    int height;
    std::vector<char> data;

public:

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;
};

