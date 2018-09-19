#include "mainwindow.h"
#include <QApplication>
#include "global.h"


int main(int argc, char *argv[])
{
    qDebug("Soft Version: %s\n", APP_VERSION);

    QApplication a(argc, argv);
    MainWindow w(argc, argv);
    w.showFullScreen();


    return a.exec();
    //return sleep(10);
}
