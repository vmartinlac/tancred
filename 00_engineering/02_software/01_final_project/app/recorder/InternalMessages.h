
#pragma once

#include <vector>
#include <QSharedPointer>
#include "Constants.h"

struct InternalImageMessage
{
    InternalImageMessage() = default;
    InternalImageMessage(const InternalImageMessage& o) = delete;
    InternalImageMessage& operator=(const InternalImageMessage& o) = delete;

    ClockType::time_point local_timestamp;
    int frameid;
    double timestamp;
    int width;
    int height;
    std::vector<char> data;
};

using InternalImageMessagePtr = QSharedPointer<InternalImageMessage>;

Q_DECLARE_METATYPE(InternalImageMessagePtr);

struct InternalMotorsMessage
{
    InternalMotorsMessage() = default;
    InternalMotorsMessage(const InternalMotorsMessage& o) = delete;
    InternalMotorsMessage& operator=(const InternalMotorsMessage& o) = delete;

    ClockType::time_point local_timestamp;
    bool forward;
    double steering;
};

using InternalMotorsMessagePtr = QSharedPointer<InternalMotorsMessage>;

Q_DECLARE_METATYPE(InternalMotorsMessagePtr);

