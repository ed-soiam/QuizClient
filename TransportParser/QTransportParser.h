#ifndef QTRANSPORTPARSER_H
#define QTRANSPORTPARSER_H
#include <QObject>

class QTransportParser : public QObject
{
    Q_OBJECT
public:
    explicit QTransportParser(QObject * parent = 0);
    //парсинг команды
    virtual QByteArray parse(const QByteArray & data) = 0;
    //создание команды
    virtual QByteArray create(const QByteArray & data) = 0;
    virtual void reset() = 0;
signals:

public slots:
};

#endif // QTRANSPORTPARSER_H
