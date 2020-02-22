#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>
#include <QPushButton>
#include "ConfigDialog.h"

ConfigDialog::ConfigDialog(QWidget* w) : QDialog(w)
{
    myUI.setupUi(this);

    connect(myUI.btn_change, SIGNAL(clicked()), this, SLOT(changeDatasetDirectory()));

    QSettings s;
    myUI.dataset_directory->setText( s.value("dataset_directory", QString("")).toString() );
}

void ConfigDialog::changeDatasetDirectory()
{
    QString ret = QFileDialog::getExistingDirectory(this, "Select dataset directory");

    if(ret.isEmpty() == false)
    {
        myUI.dataset_directory->setText(ret);
    }
}

void ConfigDialog::accept()
{
    ConfigPtr ret;
    const char* err = "";
    bool ok = true;

    if(ok)
    {
        ok = !myUI.dataset_directory->text().isEmpty();
        err = "Please set dataset directory!";
    }

    if(ok)
    {
        ret.reset(new Config());
        ret->dataset_directory = myUI.dataset_directory->text();
        ret->dataset.reset(new Dataset());
        ok = ret->dataset->reset(ret->dataset_directory);
        err = "Could not open or create dataset!";
    }

    if(ok)
    {
        QSettings s;
        s.setValue("dataset_directory", myUI.dataset_directory->text());

        myConfig = std::move(ret);

        QDialog::accept();
    }
    else
    {
        QMessageBox::critical(this, "Error", err);
    }
}

ConfigPtr ConfigDialog::askConfig(QWidget* parent)
{
    ConfigPtr ret;
    ConfigDialog* dlg = new ConfigDialog(parent);

    dlg->exec();

    ret = dlg->myConfig;

    delete dlg;

    return ret;
}

