
#pragma once

#include <QDialog>
#include "Config.h"
#include "ui_ConfigDialog.h"

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:

    static ConfigPtr askConfig(QWidget* parent=nullptr);

protected:

    void accept() override;

    ConfigDialog(QWidget* w=nullptr);

protected slots:

    void changeDatasetDirectory();

protected:

    Ui::ConfigDialog myUI;
    ConfigPtr myConfig;
};

