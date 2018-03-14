#ifndef MYMANAGERINFO_H
#define MYMANAGERINFO_H
#include <QApplication>
#include <QMutex>
#include "qthreadobject.h"
#include "TransportParser/QJSONParser.h"
#include "QThreadSocket.h"
#include "qjsontask.h"
//#include "guiprop.h"
//#include "tasks/qjsontask.h"



class MyManagerInfo : public QThreadObject
{
    Q_OBJECT
public:
    explicit MyManagerInfo(QObject *parent = 0, const QString & path = QString());
    virtual ~MyManagerInfo();
    //отправка JSON event/событие в сокет с возможностью добавления доп. информации в additional_json_fields(например QString("\"ip\":\"192.168.0.11\""))
    void sendEvent(const QString & event,const QString & additional_json_fields = QString());



signals:

public slots:

private:
    QThreadSocket socket; //Мой сокет

    QList<QJSONTask> _tasks;
    QJSONTask current_task;


};

#endif // MYMANAGERINFO_H
