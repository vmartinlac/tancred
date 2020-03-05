#include <QToolBar>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QActionGroup>
#include <QDockWidget>
#include "MainWindow.h"

MainWindow::MainWindow(RobotInterface* conn, QWidget* w) : QMainWindow(w)
{
    myConn = conn;
    myVideo = new VideoWidget(conn);

    QToolBar* tb = addToolBar("Toolbar");
    QAction* a_start_self_driving = tb->addAction("StartAutoPilot");
    QAction* a_stop_self_driving = tb->addAction("StopAutoPilot");
    tb->addSeparator();
    QAction* a_silent = tb->addAction("Silent");
    QAction* a_driving = tb->addAction("Driving");
    tb->addSeparator();
    QAction* a_about = tb->addAction("About");
    QAction* a_quit = tb->addAction("Quit");

    QActionGroup* grp = new QActionGroup(this);
    grp->addAction(a_silent);
    grp->addAction(a_driving);

    a_silent->setCheckable(true);
    a_driving->setCheckable(true);
    a_silent->setChecked(true);
    myVideo->setModeToSilent();

    /*
    {
        QVBoxLayout* l = new QVBoxLayout();
        l->addWidget(myController);

        QWidget* w = new QWidget();
        w->setLayout(l);

        QDockWidget* dock = new QDockWidget();
        dock->setWidget(w);

        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }
    */

    setCentralWidget(myVideo);
    setWindowTitle("Tancred Wizard");
    setMinimumSize(640, 480);

    connect( a_start_self_driving, SIGNAL(triggered()), this, SLOT(sendEnableAutoPilot()), Qt::QueuedConnection );
    connect( a_stop_self_driving, SIGNAL(triggered()), this, SLOT(sendDisableAutoPilot()), Qt::QueuedConnection );
    connect( a_quit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()) );
    connect( a_about, SIGNAL(triggered()), this, SLOT(about()) );
    connect( a_silent, SIGNAL(triggered()), myVideo, SLOT(setModeToSilent()) );
    connect( a_driving, SIGNAL(triggered()), myVideo, SLOT(setModeToDriving()) );
}

void MainWindow::sendEnableAutoPilot()
{
    myConn->sendEnableAutoPilot();
}

void MainWindow::sendDisableAutoPilot()
{
    myConn->sendDisableAutoPilot();
}

void MainWindow::about()
{
    QMessageBox::information(this, "About", "Developped by Victor MARTIN-LAC in 2020 as part of Tancred project.");
}

