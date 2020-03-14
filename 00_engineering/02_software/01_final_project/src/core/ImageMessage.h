
#pragma once

#include <vector>
#include <cstdint>
#include <yarp/os/Portable.h>

class ImageMessage : public yarp::os::Portable
{
public:

    enum Format
    {
        FORMAT_GRAYSCALE8,
        FORMAT_ENCODED
    };

public:

    int frameid;
    double timestamp;
    int width;
    int height;
    int format;
    std::vector<char> data;

public:

    bool read(yarp::os::ConnectionReader& reader) override;
    bool write(yarp::os::ConnectionWriter& writer) const override;
};

