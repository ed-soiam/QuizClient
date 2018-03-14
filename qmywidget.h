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
//#include "qmywidget.h"
#include "QThreadSocket.h"
//#include <QThreadSocket.h>
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


signals:
    //void finished_socket();

private:
    MyClient * connectHost;    
    QHBoxLayout* h_BoxLayout;
    QHBoxLayout* h_BoxLayout2;
    QVBoxLayout* v_BoxLayout;
    QSpacerItem* spacerItem;

    QGridLayout* gridBaseLayout;
    QToolButton* buttonConnect;
    QToolButton* buttonUpdate;
    QToolButton* buttonScript;
    QTextEdit* myTextEdit;
    QTextEdit* myTextEditEnt;
    QComboBox* comBoxIP;

    QLabel* labelIP;
    QLabel* labelReg;
    QThreadSocket* myThreadSocket;
    QThreadSocket* myThreadSocket2;
    bool startWidget=false;
    //QTask* myTask;
    QJSONTask* myJsonTask;
    QByteArray dataGetManagerInfo;
    QSHManagerInfo* managerInfo=0;
    QString commandString;
    bool connectSocket=false;    
//********************************************
//Кнопки выбора для викторины


    QList<QPushButton*> listPushButton; //Список кнопок
    QGridLayout* gridButton;
    void menuButtonGame(); //Вывод одноко из меню

//Генерация и сохранение в реестре ключа регистрации
    QSettings app_settings;
    QString settings_register_key();
    QToolButton* buttonRegister;
    bool registerQuiz=false;  //Регистрация. Возможно можно как ни будь убрать

//Тестовые
/*    QPushButton * buttonChoice; //Возможно выбор варианта меню
    QComboBox * comboBoxGame;
    bool statusGame=false;
    void load_settings(); //Извлечение настроек сохраненных в реестре
*/
public slots:
    void slotButtonConnect();  //Подключение
    void slotButtonUpdate();   //Обновление списка вожможных подключений
    void slotAddrForComboBox(QList<QHostAddress> openSocketList);
    void slotSetConnectSocket(bool value);
    void slotButtonGame();     //Отправка команд
    void slotChoiceMenu(QStringList numberMenu); //Выбора варианта меню.
    void slotButtonRegister(); //Кнопка регистрации
    void slotRegSuccessfully(bool regS);
    //void slotAddrTextEdit(QString _answer);
    //void slotButtonSendScript();
    //void slotDelWidget();
    //void slotSelectionGame();//Ручной выбор варианта игры Тест
signals:
    void signalChoiceMenu(int numberMenu);
    void signalDelWidget();





};

#endif // QMYWIDGET_H
