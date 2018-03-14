#include <QCoreApplication>
#include <QJsonObject>
#include "qshmanagerinfo.h"
//#include "guiprop.h"


QSHManagerInfo::QSHManagerInfo(QObject *,const QString & path) :
    QThreadObject(),
    socket(QThreadSocket::SOCKET_TCP)
{
    socket.setTrasportParser(new QJSONParser());
    socket.setServerName(path);    //соекету передан адрес/имя подключения
/*    QJSONTask task;                //Создана задача
    task.setTimeout(22000);
    task.setAnswerParser(QJSONTask::JSON_ANSWER_GET_GSM_PARAMS);
    task.setInfinity(true);
    task.setPeriod(60000);
    task.setTaskData(QByteArray("{\"class\":\"gsm\",\"command\":\"getModemParams\",\"gsmStatus\":true,\"signalLevel\":true, \"priority\" : 10}"));
    _tasks.append(task);        //Задача сохранена в список задач
*/
    //connect(&socket,SIGNAL(errorOccured(QString)),this,SLOT(logError(QString)));
}


QSHManagerInfo::~QSHManagerInfo()
{

}

void QSHManagerInfo::process()
{
    connect(&socket,SIGNAL(receivedMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
    connect(this,SIGNAL(writeToSocket(QByteArray)),&socket,SLOT(writeToSocket(QByteArray)));
    connect(&socket,SIGNAL(signalConnected(bool)),this,SIGNAL(setConnected(bool)),Qt::UniqueConnection);//Соединены два сигнала
    connect(this,SIGNAL(finished_socket_entr()),&socket,SLOT(disconnectSocket()));

    while (canContinue())
    {
        while (!socket.isConnected() && canContinue()) // socket.isConnected() возвращает bool _connected
            if (!socket.connectToServer())
            {
                QThread::sleep(2);
                qDebug()<< "Не подключен 45 строка";
            }

        execTasks();
        QThread::msleep(100);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    }
    emit finished();
}


void QSHManagerInfo::execTasks() //Внутренняя функция
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

void QSHManagerInfo::sendCommand(const QString & command,QJSONTask::JSON_ANSWER_WAIT parser) //Постановка задачи (команда)
{
    QJSONTask task2;
    task2.setTimeout(22000);
    task2.setInfinity(false); //Бесконечность исполнения
    task2.setTaskData(command.toUtf8());  //Устанавливает и преобразует комнаду в ByteArray
    qDebug()<<command;
    task2.setAnswerParser(parser);        //Устанваливает пар
    if (parser != QJSONTask::JSON_ANSWER_NONE)
        task2.setLabel(++_label);
    _tasks.append(task2); //Новая задача добавляется в список задач
}

void QSHManagerInfo::parseMessage(const QByteArray & data)  //Слот для команд/ответов
{   
    QJsonParseError json_error;
    //Разбирает json как документ JSON, кодированный UTF-8, и создает из него QJsonDocument.
    QJsonDocument json_doc = QJsonDocument::fromJson(data,&json_error);

    if (json_error.error != QJsonParseError::NoError) //Проверка на синтакситеческие ошибки.
        return;

    //object()- Возвращает объект QJsonObject, содержащийся в документе.
    //Возвращает пустой объект, если документ содержит массив.
    QVariantMap command_map = json_doc.object().toVariantMap();

    //Возвращает true если нашел соответствующий тип хранишища, иначе false
    //Если нет нужных типов return
    if (!command_map["command"].isValid())
        return;

    //Есть ли ответ
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
            current_task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);

        if (status == "completed")   //Завершено
        {
            //расширенный анализ ответа
            parseJSONAnswer(command_map);

            current_task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
            return;
        }

        if (status == "accepted") //Принято
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

    if ( command_map["command"].toString() == "unregister")
    {
        //parseJSONAnswer(command_map);
        emit  registerSuccessfully(false);

    }


    QString strGui=command_map["command"].toString();
    if(strGui=="setGui")
        parseJSON_Gui(command_map);
}

void QSHManagerInfo::parseJSON_Gui(const QVariantMap &answer)
{
    QList<QVariant> varButton = answer["buttons"].toList();
    QStringList strListName;
    for(int i=0;i<varButton.length();i++)
       strListName.append(varButton[i].toString());
    emit setVarMenu(strListName);
    QString strAnswer=QString("{\"command\":\"answer\",\"label\":%1,\"status\":\"completed\"}").arg(answer["label"].toString());
    sendCommand(strAnswer,QJSONTask::JSON_ANSWER_NONE);
    //qDebug()<<"Кнопки:"<<strListName;
    //qDebug()<<answer;
    //qDebug()<<"Ответ"<<strAnswer;
}

void QSHManagerInfo::parseJSONAnswer(const QVariantMap & answer)  //Расшифровка сообщения
{    
    switch (current_task.answerParser())
    {
    case QJSONTask::JSON_ANSWER_NONE:
        break;

    case QJSONTask::JSON_ANSWER_STANDARD:
        if(answer["status"].isValid())
        {

            if(answer["status"].toString()=="completed")
            {
                //Показать окно об успешной геистрации
            }
        }
    break;

    case QJSONTask::JSON_ANSWER_STANDARD_REG:
        if(answer["status"].isValid())
        {
            if(answer["status"].toString()=="completed")
            {
               emit  registerSuccessfully(true);
            }
        }
    break;



/*
    case QJSONTask::JSON_ANSWER_GET_MANAGER_INFO:
        if (answer["status"].isValid()) //Возвращает true если тип этого хранилища найден
        {
            QString _str="IN(answer):";
            //qDebug()<<answer;

            if(answer["class"].isValid())
            {                
                _str.append("\"class\":\"");
                _str.append(answer["class"].toString());
                _str.append("\",");
            }            
            if(answer["command"].isValid())
            {
                _str.append("\"command\":\"");
                _str.append(answer["command"].toString());
                _str.append("\",");
            }
            if(answer["label"].isValid())
            {
                _str.append("\"label\":");
                _str.append(answer["label"].toString());
                _str.append(",");
            }
            if(answer["index"].isValid())
            {
                _str.append("\"index\":");
                _str.append(answer["index"].toString());
                _str.append(",");
            }
            if(answer["status"].isValid())
            {
                _str.append("\"status\":\"");
                _str.append(answer["status"].toString());
                _str.append("\"");
            }
            if(answer["savedIds"].isValid())
            {
                //QString _str;
                _str.append("\"savedIds\":[{");
                QList<QVariant> status = answer["savedIds"].toList();

                QList<QVariant> myList;
                for(int i=0;i<status.length(); i++)
                {
                    QMap<QString, QVariant> myMap;
                    myMap=status[i].toMap();

                    if(myMap["id"].isValid())
                    {
                        _str.append("id:");
                        _str.append(myMap["id"].toString());
                        _str.append(",");
                    }
                    if(myMap["route"].isValid())
                    {
                        _str.append("route:");
                        _str.append(myMap["route"].toString());
                        _str.append(",");
                    }
                    if(myMap["reserved"].isValid())
                    {
                        _str.append("reserved:");
                        _str.append(myMap["reserved"].toString());
                        _str.append(",");
                    }
                    if(myMap["seved"].isValid())
                    {
                        _str.append("seved:");
                        _str.append(myMap["seved"].toString());
                        _str.append(",");
                    }
                    if(myMap["class"].isValid())
                    {
                        _str.append("class:");
                        _str.append(myMap["class"].toString());
                        _str.append("}]}");
                    }

                    //qDebug()<<_str;
                }
                //qDebug()<<_str;
                //qDebug()<<_status;
                //emit addrTextEdit(_str);
            }
            emit addrTextEdit(_str);
        }
        break;*/
    }
}

void QSHManagerInfo::finished_socket_1() //Ретранслирует закрытие соединения на верх, в виджет
{
    emit finished_socket_entr();
}

