#include <QToolBar>
#include <QMessageBox>
#include <QApplication>
#include <QActionGroup>
#include <QDockWidget>
#include "MainWindow.h"

MainWindow::MainWindow(LCMThread* conn, QWidget* w) : QMainWindow(w)
{
    myConn = conn;

    myVideo = new VideoWidget(conn);

    QToolBar* tb = addToolBar("Toolbar");
    QAction* a_start_self_driving = tb->addAction("StartSelfDriving");
    QAction* a_stop_self_driving = tb->addAction("StopSelfDriving");
    tb->addSeparator();
    QAction* a_silent = tb->addAction("Silent");
    QAction* a_manual = tb->addAction("Manual");
    QAction* a_record = tb->addAction("Record");
    tb->addSeparator();
    QAction* a_about = tb->addAction("About");
    QAction* a_quit = tb->addAction("Quit");

    QActionGroup* grp = new QActionGroup(this);
    grp->addAction(a_silent);
    grp->addAction(a_manual);
    grp->addAction(a_record);

    a_silent->setCheckable(true);
    a_manual->setCheckable(true);
    a_record->setCheckable(true);
    a_silent->setChecked(true);

    QDockWidget* dock = new QDockWidget();

    addDockWidget(Qt::BottomDockWidgetArea, dock);

    setCentralWidget(myVideo);
    setWindowTitle("Tancred Wizard");
    setMinimumSize(320, 200);

    connect( a_start_self_driving, SIGNAL(triggered()), myConn, SLOT(sendEnableSelfDriving()), Qt::QueuedConnection );
    connect( a_stop_self_driving, SIGNAL(triggered()), myConn, SLOT(sendDisableSelfDriving()), Qt::QueuedConnection );
    connect( a_quit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()) );
    connect( a_about, SIGNAL(triggered()), this, SLOT(about()) );
}

void MainWindow::about()
{
    QMessageBox::information(this, "About", "Developped by Victor MARTIN-LAC in 2020 as part of Tancred project.");
}

