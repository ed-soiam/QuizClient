#ifndef QJSONPARSER_H
#define QJSONPARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QMutex>
#include <QWaitCondition>
#include "QTransportParser.h"

class QJSONParser : public QTransportParser
{
    Q_OBJECT
public:
    explicit QJSONParser(QObject *parent = 0);
    ~QJSONParser();
    virtual QByteArray parse(const QByteArray & data);                  //Разобрать
    virtual QByteArray create(const QByteArray & data){return data;}    //Собрать
    virtual void reset();
private:
    QByteArray in;
};

#endif // QJSONPARSER_H
/*
public:
    explicit QTransportParser(QObject * parent = 0);
    //парсинг команды
    virtual QByteArray parse(const QByteArray & data) = 0;
    //создание команды
    virtual QByteArray create(const QByteArray & data) = 0;
    virtual void reset() = 0;
*/
