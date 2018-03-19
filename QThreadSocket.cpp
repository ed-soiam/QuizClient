#include "QThreadSocket.h"
#include <QApplication>
#include <QHostAddress>

QThreadSocket::QThreadSocket(SOCKET_TYPE sock_type) : QObject()
{
    _sock_type = sock_type; //В Windows это именованный канал, а в Unix это локальный доменный сокет
    unix_socket = 0;        //
    tcp_socket = 0;
    socket = 0;             //ссылка на активный сокет
    _connected = false;     //Пока не подключен
    thread = new QThread(); //Создает новый поток. Родитель берет на себя ответственность за QThread.
    moveToThread(thread);   //Перемещает обработку событий в поток thread
    thread -> start();      //Начинает выполнение потока, вызывая run ().
    connect(this,SIGNAL(signalConnected(bool)),this,SLOT(setConnected(bool))); //Слот устанавливает значение bool _connected;
    //void signalConnected(bool);//ивент подключенности сокета
    //void setConnected(bool value){_connected = value;}
}

QThreadSocket::~QThreadSocket()
{
    thread -> quit();
    thread -> wait(500);
    thread -> deleteLater();
    emit finished_socket();
}

/**
 * @brief QThreadUnixSocket::connectTo
 * Открывает сокет ( подключается к серверу ).
 * Если @param _serverName существует, текущее имя сервера будет изменено,
 * при условии что сокет не открыт в момент вызова функции
 * @param _serverName
 * @return успешность выполнения ( true false )
 */
bool QThreadSocket::connectToServer(const QString &_serverName, int port)
{

    if (socket && socket -> isOpen())
        emit finished_socket();//по идее сигнал дойдет когда-нибудь,
                                // так что можно создавать новый объект

    if (!_serverName.isEmpty()) //Имя сервера не пустое
        _name = _serverName;
    else
        if (_name.isEmpty())
            return false;   //Если имя пустое, возвращает false

    _port = port;

    switch (_sock_type)
    {
    case SOCKET_UNIX:
        unix_socket = new QLocalSocket();
        socket = unix_socket;
        unix_socket -> connectToServer(_name,QIODevice::ReadWrite);
        if (!unix_socket -> waitForConnected(2000))
        {
            unix_socket -> close(); //Закрывает устройство
            emit errorOccured(QThreadSocket::connectError);
            emit signalConnected(false);   //установка _connected=false (соединение не установлено
        }
        break;
    case SOCKET_TCP:
        tcp_socket = new QTcpSocket();
        socket = tcp_socket;

        tcp_socket -> connectToHost(QHostAddress(_name),_port,QIODevice::ReadWrite);

        if (!tcp_socket -> waitForConnected(2000)) //Ожидает подключеня в течении 2000 мс
        {
            tcp_socket -> close(); //Закрывает устройство
            emit errorOccured(QThreadSocket::connectError);
            emit signalConnected(false);   //установка _connected=false (соединение не установлено)
        }
        break;
    }

    connect(this, SIGNAL(finished_socket()), socket, SLOT(deleteLater()));
    //connect(this, SIGNAL(finished_socket()), socket, SLOT(deleteLater()));

    bool result = socket -> isOpen();

    if (result)
    {
        emit signalConnected(true);
        socket -> moveToThread(thread); // переносим его в поток QThreadObject

        connect(socket, SIGNAL(readyRead()), this, SLOT(readFromSocket()));//Подключение сигнала о готовности чтения

        switch (_sock_type)
        {
        case SOCKET_UNIX:
            connect(unix_socket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(unixErrorHandler(QLocalSocket::LocalSocketError)));
            break;
        case SOCKET_TCP:
            connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(tcpErrorHandler(QAbstractSocket::SocketError)));
            break;
        }
    }
    else
    {
        emit finished_socket();//по идее сигнал дойде когда-нибудь, так что можно создавать новый объект
        socket = 0;
    }

    return result;
}


/**
 * @brief QThreadUnixSocket::readFromSocket
 * Читает данные из сокета по сигналу о наличии
 * и эмитит сигнал receivedData
 */
void QThreadSocket::readFromSocket()  //слот для сигнала receivedData
                                       //(получены raw-данные (сырые, необработанные),
                                        //если не задан транспортный парсер

{


    if (!socket)       //Если сокет не =0, т.е. не закрыт
        return;
    if (!parser.isNull())   //Возвращает true, если обьект содержит ссылку на нулеовй указатель
    {
        QByteArray message = parser -> parse(socket -> readAll()); //readAll() - cчитывает все оставшиеся данные с устройства и возвращает его как массив байтов.
        while(!message.isEmpty())
        {
            emit receivedMessage(message);     //Полученное сообщение
            message = parser -> parse(QByteArray());
        }
    }
    else
        emit receivedData(socket -> readAll()); //Полученные данные
}


void QThreadSocket::unixErrorHandler(QLocalSocket::LocalSocketError socketError) //Обработчик ошибок unix
{
    switch (socketError)
    {
    case QLocalSocket::ServerNotFoundError:
        error = tr("The host was not found. Please check the "          //Хост не найдет. Пожалуйста
                                    "host name and port settings.");    //имя хоста и настройки порта.
        break;
    case QLocalSocket::ConnectionRefusedError:
        error = tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct.");
        break;
    case QLocalSocket::PeerClosedError:
        if (socket)
            error = tr("Server have close conection: %1.").arg(socket -> errorString());
        break;
    default:
        if (socket)
            error= tr("The following error occurred: %1.").arg(socket -> errorString());
        break;
    }
    emit errorOccured(error);
}


void QThreadSocket::tcpErrorHandler(QAbstractSocket::SocketError socketError) //Обработчик ошибок tcp
{
    switch (socketError)
    {
    case QAbstractSocket::HostNotFoundError:
        error = tr("The host was not found. Please check the "
                                    "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        error = tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct.");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        if (socket)
        {
            error = tr("Server has closed the connection: %1.").arg(socket -> errorString());
            disconnectSocket();
        }
        break;
    default:
        if (socket)
            error= tr("The following error was occurred: %1.").arg(socket -> errorString());
        break;
    }
    emit errorOccured(error);
}


/**
 * @brief QThreadUnixSocket::writeToSocket
 * Отправляет в сокет ( если подключен ) данные взятые из @param data
 * @param data
 */
void QThreadSocket::writeToSocket(QByteArray data)  //Слот для сигнала.
{
    if (!socket)
        return;
    QByteArray send_data = parser.isNull() ? data : parser -> create(data) ;

    if(parser.isNull())
        qDebug()<< "Пустой";

    if (socket -> write(send_data) == -1)
    {        
        emit errorOccured("Error occured while sending data");
        emit errorOccured(QThreadSocket::wdNotAllError);
        disconnectSocket();
        return;
    }   

    if (!socket -> waitForBytesWritten(30000))
        emit errorOccured(QThreadSocket::wdTimeoutError);


}


/**
 * @brief QThreadUnixSocket::disconnectSocket
 * отключается от сервера
 */
void QThreadSocket::disconnectSocket()
{
    socket -> close();
    emit signalConnected(false);
    emit finished_socket();
    socket = 0;
}


void QThreadSocket::setTrasportParser(QTransportParser * parser) //Задает парсер
{
    this -> parser = QSharedPointer<QTransportParser>(parser); //Задает как умную переменную
    if (this -> parser)
    {
        parser -> moveToThread(thread); //Типа, перемечает обработку событий в отдельный поток
        //this -> parser -> setParent(this);
    }
}
/*
void QThreadSocket::transferFinishedSocket()
{
    emit disconnectSocket(); //нужно еще подключить
}
*/
