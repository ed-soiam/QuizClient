#ifndef QSHMANAGERINFO_H
#define QSHMANAGERINFO_H
#include <QApplication>
#include <QMutex>
#include "myThreadSocket/qthreadobject.h"
#include "TransportParser/QJSONParser.h"
#include "myThreadSocket/QThreadSocket.h"
#include "qtask/qjsontask.h"
//#include "guiprop.h"
//#include "tasks/qjsontask.h"

class QSHManagerInfo : public QThreadObject
{
    Q_OBJECT
public:
    explicit QSHManagerInfo(QObject *parent = 0, const QString & path = QString()); //Конструктор
    virtual ~QSHManagerInfo();
    //отправка JSON event/событие в сокет с возможностью добавления доп. информации в additional_json_fields(например QString("\"ip\":\"192.168.0.11\""))
    //void sendEvent(const QString & event,const QString & additional_json_fields = QString()); //Я использую sendCommand
    //Отправка команды, аналог сигнал/слот
    void sendCommand(const QString & command,QJSONTask::JSON_ANSWER_WAIT parser = QJSONTask::JSON_ANSWER_NONE);
    void finished_socket_1();

signals:
    void writeToSocket(QByteArray);
    void setConnected(bool);
//    void finished_socket_entr();
    void setVarMenu(QStringList _strListMenu); //Сигнал с вариантом меню для игры
    void registerSuccessfully(bool regS);      //Сигнал со статусом регистрации

private slots:
    void parseMessage(const QByteArray & data); //Слот/первичный парсер сообщений от сервера

private:
    virtual void process(); //Виртуальный слот переопределенный от родителя. вызывается при удачном подключении сокет
    void execTasks();       //Выполнение задчач из списка. Список нужен для последовательной обработки большого количества задач (при необходимости).
    void parseJSONAnswer(const QVariantMap & answer); //Расшифровка сообщения
    void parseJSON_Gui(const QVariantMap & answer);   //Расшифровка сообщения Gui, наверное, нужно будет добавить в общий парсер

    QThreadSocket socket;

    //задачи для менеджера, парсер ожидания текущего ответа и время таймаута
    QList<QJSONTask> _tasks; //Список текущих задачь
    QJSONTask current_task;  //Текущая задача - которая выполняетя в данный момент

    QMutex local_mutex; //Класс QMutex обеспечивает сериализацию доступа между потоками.
    int _label = 1;
//    bool _gsm_inited = false;

};

#endif // QSHMANAGERINFO_H
