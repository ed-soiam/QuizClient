#ifndef QMYWIDGET_H
#define QMYWIDGET_H

#include <QWidget>
#include "mainwindow.h"
#include <QMainWindow>
#include <QApplication>
#include "MyClient.h"
#include <QTcpSocket>
#include <QtWidgets>
#include "myclient.h"
#include "QThreadSocket.h"
#include "qtask.h"
#include "qjsontask.h"
#include "qshmanagerinfo.h"
#include <QSettings>
#include <QSizePolicy>
#include <QPushButton>
#include <QSpacerItem>

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
    QTextEdit* myTextEdit;
    QComboBox* comBoxIP;
    QLabel* labelIP;
    QSHManagerInfo* managerInfo=0;//Подключение
    bool connectSocket=false;
    QSettings app_settings;
    QString settings_register_key();//Генерация и сохранение в реестре ключа регистрации
    QSharedPointer<QLabel> labelImage;
//    QScopedPointer<QLabel> labelImage;
//    QPointer<QLabel> labelImage;
    QList<QString> strNameImage;

public slots:
    void slotButtonConnect();  //Подключение
    void slotButtonUpdate();   //Обновление списка вожможных подключений
    void slotAddrForComboBox(QList<QHostAddress> openSocketList);
    void slotSetConnectSocket(bool value); //Наличие подключения
    void slotButtonGame();     //Отправка команд
    void slotChoiceMenu(QStringList numberMenu); //Выбора варианта меню.
    void slotButtonRegister(); //Кнопка регистрации
    void slotRegSuccessfully(bool regS); //Наличие регистрации

signals:
    void signalChoiceMenu(int numberMenu);

};

#endif // QMYWIDGET_H
