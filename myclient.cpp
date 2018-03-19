#include "myclient.h"
#include "mainwindow.h"
#include "qmywidget.h"



MyClient::MyClient()//: QObject()
{
    for (int i=0; i<256; i++){                     // Создание указателей для 255 отдельных подключений
        m_pTcpSocket = new MyTcpSocket;            //Создание указателя для отдельного соединения
        socketList.append(m_pTcpSocket);}          //Сохранение указателей в контейнер

    for(int i=0; i < socketList.length();i++ )
           connect (socketList.at(i), SIGNAL(connected()),this, SLOT(slotConnected())); //Соединение установлено

    for(int i=0; i < socketList.length();i++ )
           connect (socketList.at(i), SIGNAL(readyRead()),this, SLOT(slotReadyRead())); //Соединение готово к чтению

    for(int i=0; i < socketList.length();i++ )
        connect(socketList.at(i), SIGNAL(signalAddrOpenPort(QHostAddress)),
            this, SLOT(slotAddrOpenPort(QHostAddress)) //Добавить адрес открытого порта
           );

    for(int i=0; i < socketList.length();i++ )
        connect(socketList.at(i), SIGNAL(signalAddrClosedPort(QHostAddress)),
            this, SLOT(slotAddrClosedPort(QHostAddress)) //Добавить адрес закрытого порта
           );

    myHostAddress();                    //Определение своего IP
    myConnectHost();                    //Попытка подключения к всем возможным адресам сети
}
/*
MyClient::~MyClient()
{

}
*/
//****************************************************************************************
//Определение своего адреса в локальной сети

void MyClient::myHostAddress()
{
    QList<QHostAddress> addr_list;                      // Для сохранения всех возможных подключений к локальной сети
    QHostAddress templateHostAddress("192.168.0.0");                   // Шаблон для определения правильного IP "192.168.0.0"
    quint32 uncertainIP;                                //Промежуточный IP подогнанный под шаблон

    foreach(QHostAddress addr,QNetworkInterface::allAddresses()) //Эта функция возвращает все IP-адреса, найденные на главной машине.
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost)
            addr_list.append(addr);

    for(int i=0; i<addr_list.length(); ++i)
        {
            uncertainIP = addr_list[i].toIPv4Address() & 0xffff0000;

            if(uncertainIP == templateHostAddress.toIPv4Address())
            {                 
                locIP= addr_list[i].toIPv4Address() & 0xffffff00;
            }
        }
}
//**********************************************************************
//Подключение к сокетам
void MyClient:: myConnectHost()
{
    for(int i=0; i < socketList.length();i++ )
           connect (this, SIGNAL(waitConnectSocket()),socketList.at(i), //Подключение
                    SLOT(slotWaitConnect()), Qt::QueuedConnection);

    for(int i=1; (i<socketList.length()); i++)       //Подключение к 255 сокетам
    {        
        newLocIP= QHostAddress(locIP+i);
        socketList.at(i)->setPeerAddressMy(newLocIP, nPort);
    }

       emit waitConnectSocket(); //Сигнал на подключение сразу для всех сокетов
}

//****************************************************************************

void MyClient::slotAddrOpenPort(QHostAddress addrOpenPort) //Учет открытых портов
{
    openSocketList.append(addrOpenPort);
}

void MyClient::slotAddrClosedPort(QHostAddress addrClosedPort) //Учет закрытых портов
{        
    errSocketList.append(addrClosedPort);
    totalHost=openSocketList.length()+ errSocketList.length();

    if(totalHost==256)
    {
       emit signalAddrForComboBox(openSocketList);
       openSocketList.clear();
       errSocketList.clear();
       totalHost=0;
    }    
}

void MyClient::updateConnectHost()
{
    emit waitConnectSocket(); //Сигнал для подключения всех созданных сокетов
}

//***************************************************************************
//Слот подключения
void MyClient::slotConnected()              //Произведено соединение
{

/*
    for(int i=0; i<socketList.length(); i++)
    {
        if(sender()==dynamic_cast<QObject*>(socketList.at(i)))  //Определение указателя объекта выславшего сигнал
        {
            openSocketList.append(socketList.at(i)->peerAddress()); //Сохраняем доступные подключения
            qDebug()<<"Сохранен адрес:"<< myTimer.elapsed();

        }
    }

    //allHostAddress.append(QHostAddress(locIP));
    //emit signalConnected(true);
    //m_ptxtInfo->append("Received the connected() signal");

    //qDebug() << m_pTcpSocket->localAddress();
    //qDebug() << newLocIP;
    //qDebug() << QHostAddress(locIP+i);

    //qDebug() << "Адрес подключения:" << newLocIPReturn();
   // m_pTcpSocket->disconnectFromHost();
*/
}

//********************************************************************************
//Слот ошибок
void MyClient::slotError(QAbstractSocket::SocketError err) //Слот slotError() вызывается при возникновении ошибок. В нем мы преобразуем код ошибки в текст для того, чтобы отобразить его в виджете многострочного текстового поля.
{
    //QString strError =
    strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "Порт не найден." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "Удаленный хост закрыт." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "Соединение было отклонено." :
                     QString(m_pTcpSocket->errorString())
                    );

}

void MyClient::slotReadyRead()    //Готовность предоставить данные для чтения
{
    qDebug() << "Готовность предоставить данные";
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;
    }
}
