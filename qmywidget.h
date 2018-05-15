#ifndef QMYWIDGET_H
#define QMYWIDGET_H

#include <QWidget>
//#include "mainwindow.h"
//#include <QMainWindow>
#include <QApplication>
#include <QTcpSocket>
#include <QtWidgets>
#include "myclient.h"
#include "myThreadSocket/QThreadSocket.h"
#include "qtask/qtask.h"
#include "qtask/qjsontask.h"
#include "myThreadSocket/qshmanagerinfo.h"
#include <QSettings>
#include <QSizePolicy>
#include <QPushButton>
#include <QSpacerItem>
#include "qtwraptextpushbutton.h"

class QMyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMyWidget(QWidget *parent = 0);
    ~QMyWidget();

private:
    MyClient * connectHost;    
    QHBoxLayout* h_BoxLayout;
    QHBoxLayout* h_BoxLayout2;
    QVBoxLayout* v_BoxLayout;
    QSpacerItem* spacerItem;
    QGridLayout* gridButton; //Сетка для вывода кнопок
    QToolButton* buttonConnect;
    QToolButton* buttonUpdate;
    QToolButton* buttonRegister;
    QList<QPushButton*> listPushButton; //Список кнопок    
    QComboBox* comBoxIP;
    QSHManagerInfo* managerInfo=0;//Подключение
    bool connectSocket=false;
    QSettings app_settings;
    QString settings_register_key();//Генерация и сохранение в реестре ключа регистрации
    QSharedPointer<QLabel> labelImage; //Умный указатель нужно инициализировать при создании конструктора или инициализировать при помощи оператора присвоения
    //в самом конструкторе (присвоение указателя локально-созданной переменной глобальной переменной класса
    //h_BoxLayout->addWidget(labelImage.data()); //Для применения умной меременной нужно использовать data()
//    QScopedPointer<QLabel> labelImage;
//    QPointer<QLabel> labelImage;
    QList<QString> strNameImage;
    void regStatus(bool statusReg);
    void buttonDeleteGame(); //Кнопки нужно удалять из слота и из самого виджета, поэтому пришлось добавить отдельную функцию


public slots:
    void slotButtonConnect();  //Подключение
    void slotButtonUpdate();   //Обновление списка вожможных подключений
    void slotAddrForComboBox(QList<QHostAddress> openSocketList);
    void slotSetConnectSocket(bool value); //Наличие подключения
    void slotButtonGame();     //Отправка команд
    void slotChoiceMenu(QStringList numberMenu); //Выбора варианта меню.
    void slotButtonRegister(); //Кнопка регистрации
    void slotRegStatus(bool regS); //Наличие регистрации

signals:
    void signalChoiceMenu(int numberMenu);

};

#endif // QMYWIDGET_H
