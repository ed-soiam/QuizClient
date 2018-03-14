#include "mainwindow.h"
#include <QApplication>
#include "qmywidget.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QString>("QString"); //Регистрация типа для создания буфера для хранения метаобьектной информации
                                            // до этого выкидывал ошибку: QObject::connect: Cannot queue arguments of type 'QAbstractSocket::SocketError'
                                            //(Make sure 'QAbstractSocket::SocketError' is registered using qRegisterMetaType().)
    qRegisterMetaType<QHostAddress>("QHostAddress");


    QApplication a(argc, argv);
    //MainWindow w;

    QMyWidget w;            // Создаём свой виджет.

    w.setWindowTitle("Викторина"); //Смена названия

    w.show();

    return a.exec();
}
