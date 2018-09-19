#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>

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
    int slotRaw2rgbWithCV(void);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
