#ifndef QSIMPLECOMMANDPARSER_H
#define QSIMPLECOMMANDPARSER_H

#include "QTransportParser.h"

class QSimpleCommandParser : public QTransportParser
{
    Q_OBJECT
public:
    explicit QSimpleCommandParser(QObject *parent = 0);
    //парсинг команды
    virtual QByteArray parse(const QByteArray & data);
    //создание команды
    virtual QByteArray create(const QByteArray & data);
    virtual void reset();
private:
    QByteArray in;
};

#endif // QSIMPLECOMMANDPARSER_H

