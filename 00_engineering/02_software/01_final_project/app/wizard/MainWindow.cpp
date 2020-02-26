#include <QToolBar>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QActionGroup>
#include <QDockWidget>
#include "MainWindow.h"

MainWindow::MainWindow(LCMThread* conn, QWidget* w) : QMainWindow(w)
{
    myConn = conn;
    myVideo = new VideoWidget(conn);
    myController = new ControllerWidget(conn);

    QToolBar* tb = addToolBar("Toolbar");
    QAction* a_start_self_driving = tb->addAction("StartSelfDriving");
    QAction* a_stop_self_driving = tb->addAction("StopSelfDriving");
    tb->addSeparator();
    QAction* a_silent = tb->addAction("Silent");
    QAction* a_driving = tb->addAction("Driving");
    QAction* a_recording = tb->addAction("Recording");
    tb->addSeparator();
    QAction* a_about = tb->addAction("About");
    QAction* a_quit = tb->addAction("Quit");

    QActionGroup* grp = new QActionGroup(this);
    grp->addAction(a_silent);
    grp->addAction(a_driving);
    grp->addAction(a_recording);

    a_silent->setCheckable(true);
    a_driving->setCheckable(true);
    a_recording->setCheckable(true);
    a_silent->setChecked(true);
    myVideo->setModeToSilent();

    {
        QVBoxLayout* l = new QVBoxLayout();
        l->addWidget(myController);

        QWidget* w = new QWidget();
        w->setLayout(l);

        QDockWidget* dock = new QDockWidget();
        dock->setWidget(w);

        addDockWidget(Qt::BottomDockWidgetArea, dock);
    }

    setCentralWidget(myVideo);
    setWindowTitle("Tancred Wizard");
    setMinimumSize(320, 200);

    connect( a_start_self_driving, SIGNAL(triggered()), myConn, SLOT(sendEnableSelfDriving()), Qt::QueuedConnection );
    connect( a_stop_self_driving, SIGNAL(triggered()), myConn, SLOT(sendDisableSelfDriving()), Qt::QueuedConnection );
    connect( a_quit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()) );
    connect( a_about, SIGNAL(triggered()), this, SLOT(about()) );
    connect( a_silent, SIGNAL(triggered()), myVideo, SLOT(setModeToSilent()) );
    connect( a_driving, SIGNAL(triggered()), myVideo, SLOT(setModeToDriving()) );
    connect( a_recording, SIGNAL(triggered()), myVideo, SLOT(setModeToRecording()) );
}

void MainWindow::about()
{
    QMessageBox::information(this, "About", "Developped by Victor MARTIN-LAC in 2020 as part of Tancred project.");
}

