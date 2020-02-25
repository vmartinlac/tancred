
#pragma once

#include <QDir>
#include <QString>
#include <memory>

class Dataset
{
public:

    Dataset();

    Dataset(const Dataset& o) = delete;

    Dataset& operator=(const Dataset& o) = delete;

    bool reset(const QString& dir);

protected:

    QDir myDir;
};

using DatasetPtr = std::shared_ptr<Dataset>;

