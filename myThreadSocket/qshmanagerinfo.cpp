#include <QCoreApplication>
#include <QJsonObject>
#include "qshmanagerinfo.h"
//#include "guiprop.h"

QSHManagerInfo::QSHManagerInfo(QObject *,const QString & path) :
    QThreadObject(),
    socket(QThreadSocket::SOCKET_TCP)
{
    socket.setTrasportParser(new QJSONParser());
    socket.setServerName(path);
    QJSONTask task;
    task.setTimeout(22000);
    task.setAnswerParser(QJSONTask::JSON_ANSWER_GET_GSM_PARAMS);
    task.setInfinity(true);
    task.setPeriod(60000);
    task.setTaskData(QByteArray("{\"class\":\"gsm\",\"command\":\"getModemParams\",\"gsmStatus\":true,\"signalLevel\":true, \"priority\" : 10}"));
    _tasks.append(task);
}


QSHManagerInfo::~QSHManagerInfo()
{

}


void QSHManagerInfo::process()
{
    connect(&socket,SIGNAL(receivedMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
    connect(this,SIGNAL(writeToSocket(QByteArray)),&socket,SLOT(writeToSocket(QByteArray)));
    connect(&socket,SIGNAL(signalConnected(bool)),this,SIGNAL(setConnected(bool)));
    connect(&socket,SIGNAL(errorOccured(QString)),this,SLOT(logError(QString)));
    while (canContinue())
    {
        while (!socket.isConnected() && canContinue())
            if (!socket.connectToServer())
            {
                if (log)
                    *log << LogBuffer::LOG_LEVEL_ERROR << QString("can't connect to %1\n").arg(socket.serverName());
                QThread::sleep(2);
            }
            else
                if (log)
                    *log << LogBuffer::LOG_LEVEL_INFO << QString("connection to %1 has been established\n").arg(socket.serverName());

        execTasks();
        QThread::msleep(100);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    }
}


void QSHManagerInfo::execTasks()
{
    QMutexLocker locker(&local_mutex);
    if (current_task.answerParser() != QJSONTask::JSON_ANSWER_NONE)
    {
        if (!current_task.isTimeout())
            return;
        //TODO: можно как-нибудь отобразить таймаут
        current_task.setError(0);
        if (current_task.retryCount())
        {
            if (log)
                *log << LogBuffer::LOG_LEVEL_DEBUG << "send: " << QString(current_task.taskData()) << std::endl;
            emit writeToSocket(current_task.taskData());
        }
        else
            current_task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
    }

    for (QList<QJSONTask>::iterator it = _tasks.begin(); it != _tasks.end(); ++it)
    {
        if (!it -> isReady())
            continue;
        it -> execute();
        current_task = *it;

        current_task.setLabel(_label++);
        if (log)
            *log << LogBuffer::LOG_LEVEL_DEBUG << "send: " << QString(current_task.taskData()) << std::endl;
        emit writeToSocket(current_task.taskData());
        if (!current_task.infinity())
            _tasks.erase(it);
        break;
    }
}


void QSHManagerInfo::sendEvent(const QString & event,const QString & additional_json_fields)
{
    QMutexLocker locker(&local_mutex);
    QJSONTask task;
    task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
    task.setInfinity(false);
    QString send_data = QString("{\"class\":\"service\",\"command\":\"eventCommand\",\"event\":\"%1\"%2}").arg(event,additional_json_fields.isEmpty() ? QString() : (QString(",") + additional_json_fields));
    task.setTaskData(QByteArray(send_data.toLatin1()));
    _tasks.append(task);
}


void QSHManagerInfo::parseMessage(const QByteArray & data)
{
    QJsonParseError json_error;
    QJsonDocument json_doc = QJsonDocument::fromJson(data,&json_error);
    if (json_error.error != QJsonParseError::NoError)
        return;
    QVariantMap command_map = json_doc.object().toVariantMap();
    if (!command_map["class"].isValid() || !command_map["command"].isValid())
        return;

    if ( command_map["command"].toString() == "answer")
    {
        if (current_task.answerParser() == QJSONTask::JSON_ANSWER_NONE)
            return;
        if (!command_map["label"].isValid())
            return;
        if (command_map["label"].toInt() != current_task.label())
            return;
        //Обслуживание ответов
        if (!command_map["status"].isValid())
            return;
        QString status = command_map["status"].toString();
        if (status == "error")
        {
            if (log)
                *log << LogBuffer::LOG_LEVEL_ERROR << "get: " << QString(data) << std::endl;
            //todo: обработчик ошибок
            current_task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
            if (command_map["class"].toString() == "gsm")
            {
                _gsm_inited = true;
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::connected,false,true);
            }
        }
        if (status == "completed")
        {
            if (log)
                *log << LogBuffer::LOG_LEVEL_DEBUG << "get: " << QString(data) << std::endl;
            //расширенный анализ ответа
            parseJSONAnswer(command_map);
            current_task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
            return;
        }
        if (status == "accepted")
        {
            current_task.execute();//обноляем задачу по таймауту
            return;
        }

        if (status == "started" || status == "progress")
        {
            current_task.execute();//обноляем задачу по таймауту
            return;
        }
        return;
    }


    //обслуживание service сообщений
    if (command_map["class"].toString() == "service")
    {
        if (log)
            *log << LogBuffer::LOG_LEVEL_DEBUG << "get: " << QString(data) << std::endl;
        if (command_map["command"].toString() == "eventCommand" && command_map["event"].isValid())
        {
            QString event = command_map["event"].toString();
            if (event == "WPSTimerStarted")
            {
                int time;
                if (!command_map["time"].isValid() || (time = command_map["time"].toInt()) == 0)
                    time = 30;
                emit WPSStarted(time);
                return;
            }
            if (event == "WPSTimerStopped")
            {
                emit WPSStopped();
                return;
            }
        }
    }


    //обслуживание netro сообщений
    if (command_map["class"].toString() == "netro" || command_map["class"].toString() == "zwave")
    {
        if (command_map["command"].toString() == "eventCommand" && command_map["event"].isValid())
        {
            if (log)
                *log << LogBuffer::LOG_LEVEL_ERROR << "get: " << QString(data) << std::endl;
            QString event = command_map["event"].toString();
            if (event == "batteryPowerOn")
            {
                QGuiProps::getInstance() -> setValue(QGuiProps::battery, QGuiProps::NO_INDEX, QGuiProps::connected,true, true);
                return;
            }
            if (event == "acPowerOn")
            {
                QGuiProps::getInstance() -> setValue(QGuiProps::ac, QGuiProps::NO_INDEX, QGuiProps::connected,true, true);
                return;
            }
            if (event == "buildingNetworkStarted")
            {
                QGuiProps::getInstance() -> setValue(QGuiProps::device_network, QGuiProps::index_build_network, QGuiProps::state,QGuiProps::state_started, true);
                return;
            }
            if (event == "buildingNetworkFinished")
            {
                QGuiProps::getInstance() -> setValue(QGuiProps::device_network, QGuiProps::index_build_network, QGuiProps::state,QGuiProps::state_finished, true);
                return;
            }
            if (event == "destroyingNetworkStarted")
            {
                QGuiProps::getInstance() -> setValue(QGuiProps::device_network, QGuiProps::index_destroy_network, QGuiProps::state,QGuiProps::state_started, true);
                return;
            }
            if (event == "destroyingNetworkFinished")
            {
                QGuiProps::getInstance() -> setValue(QGuiProps::device_network, QGuiProps::index_destroy_network, QGuiProps::state,QGuiProps::state_finished, true);
                return;
            }
        }
    }
}


void QSHManagerInfo::parseJSONAnswer(const QVariantMap & answer)
{
    switch (current_task.answerParser())
    {
    case QJSONTask::JSON_ANSWER_NONE:
        break;
    case QJSONTask::JSON_ANSWER_GET_GSM_PARAMS:
        QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::connected,true);
        if (answer["gsmStatus"].isValid())
        {
            QVariantMap status = answer["gsmStatus"].toMap();
            if (status["SIM ready"].isValid() && status["SIM ready"].type() == QVariant::Bool)
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::state,status["SIM ready"].toBool());
            else
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::state,false);
            if (status["Operator name"].isValid())
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::caption,status["Operator name"].toString());
            else
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::caption,QString());
        }

        if (answer["signalLevel"].isValid())
        {
            QVariantMap level_map = answer["signalLevel"].toMap();
            if (level_map["value"].isValid() && (level_map["value"].type() == QVariant::Int || level_map["value"].type() == QVariant::Double))
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::level,gsmLevel(level_map["value"].toInt()));
            else
                QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::level,QGuiProps::quality_fail);
        }
        else
            QGuiProps::getInstance() -> setValue(QGuiProps::gsm, QGuiProps::NO_INDEX, QGuiProps::level,QGuiProps::quality_fail);
        current_task.setOk();
        _gsm_inited = true;

        QGuiProps::getInstance() -> updateFinished(QGuiProps::gsm, QGuiProps::NO_INDEX);
        break;
    }
}


void QSHManagerInfo::logError(const QString & err)
{
    if (log)
        *log << LogBuffer::LOG_LEVEL_ERROR << err << std::endl;
}


QGuiProps::EStatePropGSMQuality QSHManagerInfo::gsmLevel(int dbm_level)
{
    if (dbm_level < -105)
        return QGuiProps::quality_fail;
    if (dbm_level < -100)
        return QGuiProps::quality_bad;
    if (dbm_level < -90)
        return QGuiProps::quality_below_average;
    if (dbm_level < -80)
        return QGuiProps::quality_average;
    if (dbm_level < -65)
        return QGuiProps::quality_good;
    return QGuiProps::quality_excellent;
}

