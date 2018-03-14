#include "mymanagerinfo.h"
#include <QCoreApplication>
#include <QJsonObject>


MyManagerInfo::MyManagerInfo(QObject *,const QString & path) : QThreadObject(),
    socket(QThreadSocket::SOCKET_TCP)
{
    socket.setTrasportParser(new QJSONParser()); //Устанавливает парсер и кидает ему в кач аргумента обьект парсера
    //Нужно будет созадать свой парсер
    socket.setServerName(path); //Задает имя сервера/арес в сети

    QJSONTask task;
    task.setTimeout(22000);   //Задает таймамцт 22 сек
    task.setAnswerParser(QJSONTask::JSON_ANSWER_GET_MANAGER_INFO); //Нужно поискать где он применяется
    task.setInfinity(true); //QTask, бесконечность исполнения
    task.setPeriod(60000);  //QTask
    task.setTaskData(QByteArray(" { \"label\": 1234321, \"class\": \"service\", \"command\": \"getManagerInfo\"}"));
    //Задали данные
    _tasks.append(task); //Закинули в QList<QJSONTask>. Зачем?

}

MyManagerInfo::~MyManagerInfo()
{

}


void MyManagerInfo::sendEvent(const QString &event, const QString &additional_json_fields)
{
    QMutexLocker locker(&local_mutex);
    QJSONTask task;
    task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
    task.setInfinity(false);
    QString send_data = QString("{\"class\":\"service\",\"command\":\"eventCommand\",\"event\":\"%1\"%2}").arg(event,additional_json_fields.isEmpty() ? QString() : (QString(",") + additional_json_fields));
    task.setTaskData(QByteArray(send_data.toLatin1()));
    _tasks.append(task);
}

