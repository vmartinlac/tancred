
#pragma once

#include <memory>
#include "Dataset.h"

class Config
{
public:

    QString dataset_directory;
    DatasetPtr dataset;
};

using ConfigPtr = std::shared_ptr<Config>;

