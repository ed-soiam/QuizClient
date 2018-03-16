#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QTcpSocket>
#include <QString>
#include <QTime>
#include <QtWidgets>
#include <QList>
#include <mytcpsocket.h>

//Поиск открытых портов для подключения
class MyClient : public QObject
{
    Q_OBJECT

public:
    MyClient();
    void updateConnectHost();

private:
    void myConnectHost();
    void myHostAddress();
    quint32 locIP;                      //Собственный адрес клиента
    QHostAddress newLocIP;
    QString strError;
    MyTcpSocket * m_pTcpSocket;         //Начатльный указатель для создания 255 сокетов
    QList <MyTcpSocket*> socketList;    //Контейнер для хранения указателей на обьекты для 255 сокетов
    QList <QHostAddress> openSocketList;    //Списак доступных адресов
    QList <QHostAddress> errSocketList;    //Список недоступных адресов
    quint16 m_nNextBlockSize;
    int nPort=1234;
    int totalHost=0; //общее количество ответов от портов. 256 сигналов, отправляется сообщение с открытыми адресами

signals:
    //сигнал об изменении статуса подключения к хосту(true-подключено, false- отключено)
    void signalConnected(bool);
    void waitConnectSocket();                           //Сигнал для подключения всех созданных сокетов
    void updateConnectSocket();
    void signalAddrForComboBox(QList <QHostAddress> AddrOpenPort); //Сигнал пересылает адреса открытх портов в ComboBox
                                                                    //при получении 255 ответов от сокетов
private slots:
    void slotReadyRead   (                            ); //Готовность предоставить данные для чтения
    void slotError       (QAbstractSocket::SocketError); //Возникла ошибка
    void slotConnected   (                            ); //Прозиведено соединение
    void slotAddrOpenPort(QHostAddress addrOpenPort);    //Прием доступного адреса
    void slotAddrClosedPort(QHostAddress addrClosedPort);//Прием недоступного адреса
    //void updateSocket();

public slots:
};

#endif // MYCLIENT_H


