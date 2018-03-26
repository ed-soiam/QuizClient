#include <QCoreApplication>
#include <QJsonObject>
#include "qshmanagerinfo.h"
//#include "guiprop.h"


QSHManagerInfo::QSHManagerInfo(QObject *,const QString & path) : //Задается адрес для подключения
    QThreadObject(),                    //Указан родительский клас, а после него уже список инициализации
    socket(QThreadSocket::SOCKET_TCP) //Для инициализации полей, а также базовых классов используется список инициализации. Список инициализации отделён от заголовка класса двоеточием и имеет вид "имя поля или базового класса (значение)".
  //Данный способ позволит инициализировать заданными значениями базовые классы, константные поля, а также поля-ссылки.
{
    socket.setTrasportParser(new QJSONParser()); //Инициализируется объектом QJSONParser прямо тут. В этом месте можно выбрать любой из трех доступных парсеров в папке TransportParser
    socket.setServerName(path);                  //сокету передан адрес/имя подключения
}

void QSHManagerInfo::sendCommand(const QString & command,QJSONTask::JSON_ANSWER_WAIT parser) //Постановка задачи (команда)
{
    QJSONTask task2;                        //Создается задача
    task2.setTimeout(22000);                //Задается задержка между запусками
    task2.setInfinity(false);               //Бесконечность исполнения
    task2.setTaskData(command.toUtf8());    //Устанавливает и преобразует комнаду в ByteArray
    task2.setAnswerParser(parser);          //Устанваливает парсера
    if (parser != QJSONTask::JSON_ANSWER_NONE)//_label инициализируется только в случае если нужет ответ на команду.
        task2.setLabel(++_label);
    _tasks.append(task2); //Новая задача добавляется в список задач
}

QSHManagerInfo::~QSHManagerInfo()
{

}

void QSHManagerInfo::process() //Виртуальный слот переопределенный от родителя. вызывается при старте потока сигналом started()
{
    connect(&socket,SIGNAL(receivedMessage(QByteArray)),this,SLOT(parseMessage(QByteArray))); //Получено сообщение сокета. Обработано транспортным парсером.
    connect(this,SIGNAL(writeToSocket(QByteArray)),&socket,SLOT(writeToSocket(QByteArray)));
    connect(&socket,SIGNAL(signalConnected(bool)),this,SIGNAL(setConnected(bool)),Qt::UniqueConnection);//Соединены два сигнала. Сообщение для виджета

    while (canContinue()) //Возвращает !f_doStop=true при вызове деструктора родителя, т.е. при уничтожении QSHManagerInfo цикл будет остановлен
    {
        while (!socket.isConnected() && canContinue()) // socket.isConnected() возвращает bool _connected
            if (!socket.connectToServer()) //Если сокет открыт, пропускает. Подключает и возвращает результат. Фактически ожидает подключения.
            {
                QThread::sleep(2);   //Заставляет текущий поток спать в секундах.
            }

        execTasks(); //Выполняет текущую задачу из списка, после чего удаляет ее
        QThread::msleep(100);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1); //Обработает все ожидающие события для вызывающего потока в соответствии с указанными флажками, пока не будет больше событий для обработки.
    }
    emit finished(); //Остановка потока родителя. !f_doStop=false
}

void QSHManagerInfo::execTasks() //Выполнение поставленной задачи
{
    QMutexLocker locker(&local_mutex);  //Блокировка и разблокировка QMutex в сложных функциях и операторах или в коде обработки исключений подвержена ошибкам и трудно отлаживается. QMutexLocker может использоваться в таких ситуациях, чтобы гарантировать, что состояние мьютекса всегда четко определено.
    //Например, эта сложная функция блокирует QMutex при вводе функции и разблокирует мьютекс во всех точках выхода:
    if (current_task.answerParser() != QJSONTask::JSON_ANSWER_NONE) //Если текущая задача, требуется ответ.
    {
        if (!current_task.isTimeout()) //Учитывает заданный таймаут task2.setTimeout(22000);
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

    for (QList<QJSONTask>::iterator it = _tasks.begin(); it != _tasks.end(); ++it) //Перебирает все задачи в списке и выполняет
    {
        //проверка условия: если указатель *_pre_check не активен выполнение задачи заканчивается
        //если время последнего запуска 0, задача продолжает выполняться.
        //если время последнего запуска + время повтора выполнения задачи <= времени с момента создания задачи, то задача продолжает выполняться.
        //в проекте Quiz повтор нигде не задается
        if (!it -> isReady()) //
            continue;

        it -> execute(); //Устанавливает время с последнего запуска _last_exec = _task_timer.elapsed();
                         //в проекте Quiz повтор нигде не используется

        current_task = *it; //Текущая задача взята из списка задач для выполнения. Разыменование итератора

        emit writeToSocket(current_task.taskData()); //собщение с данными в сокет

        if (!current_task.infinity())
            _tasks.erase(it);        //Стирает из списка задачь, задачу с данным указателем
        break;
    }
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
    }
}

/*
void QSHManagerInfo::sendEvent(const QString & event,const QString & additional_json_fields)
{
    QMutexLocker locker(&local_mutex); //Например, эта сложная функция блокирует QMutex при вводе функции и разблокирует мьютекс во всех точках выхода:
    QJSONTask task;
    task.setAnswerParser(QJSONTask::JSON_ANSWER_NONE);
    task.setInfinity(false);
    QString send_data = QString("{\"class\":\"service\",\"command\":\"eventCommand\",\"event\":\"%1\"%2}").arg(event,additional_json_fields.isEmpty() ? QString() : (QString(",") + additional_json_fields));
    task.setTaskData(QByteArray(send_data.toLatin1()));
    _tasks.append(task);
}
*/

/*
void QSHManagerInfo::finished_socket_1() //Ретранслирует закрытие соединения на верх, в виджет
{
    emit finished_socket_entr();
}
*/
