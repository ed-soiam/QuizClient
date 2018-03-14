#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "myclient.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //MyClient * connection;
};

#endif // MAINWINDOW_H
