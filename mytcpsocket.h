#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QTcpSocket>
#include <QThread>
#include <QHostAddress>



class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = 0);
    void setPeerAddressMy(QHostAddress & addrHost, int & nPort);
private:
    QThread myThread;
    QHostAddress addrTcpThread;
    int myPort;

public slots:
    void slotWaitConnect();

signals:
    void signalAddrOpenPort(QHostAddress addrOpenPort);  //Передача доступного адреса
    void signalAddrClosedPort(QHostAddress addrClosedPort);

};

#endif // MYTCPSOCKET_H
