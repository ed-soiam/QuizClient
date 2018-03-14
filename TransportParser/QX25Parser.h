#ifndef QX25PARSER_H
#define QX25PARSER_H

#include <QObject>
#include "QTransportParser.h"

#define X25_START_BYTE 0x7e
#define X25_STUFF_BYTE 0x7d

#define X25_BYTE_SIZE_OFFSET 4

class QX25Parser : public QTransportParser
{
    Q_OBJECT
public:
    explicit QX25Parser(QObject *parent = 0);
    virtual QByteArray parse(const QByteArray & data);
    virtual QByteArray create(const QByteArray & data);
    virtual void reset();
    static uint16_t crc16(const uint8_t * data, unsigned char len);
private:
    QByteArray in;
    bool staf_flag;
};

#endif // QX25PARSER_H
