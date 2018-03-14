#ifndef QTHREADUNIXSOCKET_H
#define QTHREADUNIXSOCKET_H

#include "QObject"
#include <QThread>
#include <QLocalSocket>
#include <QTcpSocket>
#include <QSharedPointer>
#include "TransportParser/QTransportParser.h"

class QThreadSocket : public QObject
{
    Q_OBJECT
public:
    typedef enum{
        connectError=1,
        listenError,
        wdNotAllError,
        wdTimeoutError
    }EThreadUnixSocketError;

    typedef enum {
        SOCKET_UNIX,
        SOCKET_TCP
    }SOCKET_TYPE;

    explicit QThreadSocket(SOCKET_TYPE sock_type = SOCKET_UNIX);
    virtual ~QThreadSocket();
    bool connectToServer( const QString& _serverName = QString(), int port = 1234);
    QString serverName() const{ return _name;}
    void setServerName(const QString &value){_name = value;}
    QString lastError() const{return error;}
    /*
     * Настройка транспортного парсера входящих данных
     * Объект класса становится владельцем парсера(т.е. сам его удаляет при разрушении и смене на новый)
     */
    void setTrasportParser(QTransportParser * parser);

    bool isConnected() const {return _connected;}
signals:
    void finished_socket();
    void receivedData(const QByteArray&);//получены raw-данные, если не задан транспортный парсер
    void receivedMessage(const QByteArray&);//получено сообщение после транспортного парсера(если задан)
    void errorOccured( const QString & );
    void errorOccured( QThreadSocket::EThreadUnixSocketError );
    void signalConnected(bool);//ивент подключенности сокета
private slots:
    void setConnected(bool value){_connected = value;}
    void readFromSocket();
    void unixErrorHandler(QLocalSocket::LocalSocketError socketError);
    void tcpErrorHandler(QAbstractSocket::SocketError socketError);
public slots:
    void writeToSocket(QByteArray data);
    void disconnectSocket();
private:

    QLocalSocket * unix_socket;
    QTcpSocket* tcp_socket;
    QIODevice * socket;//ссылка на активный сокет
    QString error;
    QThread* thread;
    QSharedPointer <QTransportParser> parser;
    QString _name;//имя сокета(адрес)
    int _port;
    SOCKET_TYPE _sock_type;
    bool _connected;
};

#endif // QTHREADUNIXSOCCET_H
