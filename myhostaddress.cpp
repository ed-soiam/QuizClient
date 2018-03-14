#include "myhostaddress.h"

MyHostAddress::MyHostAddress(QObject *parent) : QObject(parent)
{
    QList<QHostAddress> addr=QNetworkInterface::allAddresses();
    QList<QHostAddress> addr_list;
    quint32 locIP, newLocIP, templateHost_quint32;
    QHostAddress templateHostAddress, myHost;
    templateHostAddress= QHostAddress("192.168.0.0");
    templateHost_quint32= templateHostAddress.toIPv4Address();

    foreach(QHostAddress addr,QNetworkInterface::allAddresses())
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost)
            addr_list.append(addr);

    for(int i=0; i<addr_list.length(); ++i)
        {
        locIP= addr_list[i].toIPv4Address();
        newLocIP= locIP & 0xffff0000;

        if(newLocIP==templateHost_quint32)
        {
         myHost= addr_list[i];
         qDebug() << myHost;
        }

        }

}
