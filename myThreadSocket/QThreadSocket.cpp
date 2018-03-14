#include "QThreadSocket.h"
#include <QApplication>
#include <QHostAddress>

QThreadSocket::QThreadSocket(SOCKET_TYPE sock_type) : QObject()
{
    _sock_type = sock_type;
    unix_socket = 0;
    tcp_socket = 0;
    socket = 0;
    _connected = false;
    thread = new QThread();
    moveToThread(thread);
    thread -> start();
    connect(this,SIGNAL(signalConnected(bool)),this,SLOT(setConnected(bool)));
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
        emit finished_socket();//по идее сигнал дойде когда-нибудь, ак что можно создавать новый объект

    if (!_serverName.isEmpty())
        _name = _serverName;
    else
        if (_name.isEmpty())
            return false;

    _port = port;

    switch (_sock_type)
    {
    case SOCKET_UNIX:
        unix_socket = new QLocalSocket();
        socket = unix_socket;
        unix_socket -> connectToServer(_name,QIODevice::ReadWrite);
        if (!unix_socket -> waitForConnected(2000))
        {
            unix_socket -> close();
            emit errorOccured(QThreadSocket::connectError);
            emit signalConnected(false);
        }
        break;
    case SOCKET_TCP:
        tcp_socket = new QTcpSocket();
        socket = tcp_socket;
        tcp_socket -> connectToHost(QHostAddress(_name),_port,QIODevice::ReadWrite);
        if (!tcp_socket -> waitForConnected(2000))
        {
            tcp_socket -> close();
            emit errorOccured(QThreadSocket::connectError);
            emit signalConnected(false);
        }
        break;
    }
    connect(this, SIGNAL(finished_socket()), socket, SLOT(deleteLater()));

    bool result = socket -> isOpen();
    if (result)
    {
        emit signalConnected(true);
        socket -> moveToThread(thread); // он переносим его в потом QThreadObject
        connect(socket, SIGNAL(readyRead()), this, SLOT(readFromSocket()));
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
        emit finished_socket();//по идее сигнал дойде когда-нибудь, ак что можно создавать новый объект
        socket = 0;
    }
    return result;
}


/**
 * @brief QThreadUnixSocket::readFromSocket
 * Читает данные из сокета по сигналу о наличии
 * и эмитит сигнал receivedData
 */
void QThreadSocket::readFromSocket()
{
    if (!socket)
        return;
    if (!parser.isNull())
    {
        QByteArray message = parser -> parse(socket -> readAll());
        while(!message.isEmpty())
        {
            emit receivedMessage(message);
            message = parser -> parse(QByteArray());
        }
    }
    else
        emit receivedData(socket -> readAll());
}


void QThreadSocket::unixErrorHandler(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
    case QLocalSocket::ServerNotFoundError:
        error = tr("The host was not found. Please check the "
                                    "host name and port settings.");
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


void QThreadSocket::tcpErrorHandler(QAbstractSocket::SocketError socketError)
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
void QThreadSocket::writeToSocket(QByteArray data)
{
    if (!socket)
        return;
    QByteArray send_data = parser.isNull() ? data : parser -> create(data) ;
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


void QThreadSocket::setTrasportParser(QTransportParser * parser)
{
    this -> parser = QSharedPointer<QTransportParser>(parser);
    if (this -> parser)
    {
        parser -> moveToThread(thread);
        this -> parser -> setParent(this);
    }
}
