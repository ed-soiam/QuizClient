#ifndef MYHOSTADDRESS_H
#define MYHOSTADDRESS_H

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QString>
#include <QtWidgets>

class MyHostAddress : public QObject
{
    Q_OBJECT
public:
    explicit MyHostAddress(QObject *parent = 0);

signals:

public slots:
};

#endif // MYHOSTADDRESS_H
