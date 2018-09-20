#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QTimer>

#include "global.h"
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char* argv[], QWidget *parent = 0);
    ~MainWindow();

public slots:
    int slotOpencvRealtimeShowRaw();
private:
    Ui::MainWindow *ui;
    QTimer dspTimer;
};

#endif // MAINWINDOW_H
