#include <QDockWidget>
#include "MainWindow.h"

MainWindow::MainWindow(lcm::LCM* conn, QWidget* w) : QMainWindow(w)
{
    myVideo = new VideoWidget(conn);

    QDockWidget* dock = new QDockWidget();

    {
        /*
            
        */
    }

    addDockWidget(Qt::BottomDockWidgetArea, dock);

    setCentralWidget(myVideo);
    setWindowTitle("Tancred Human Driver");
    setMinimumSize(320, 200);
}

