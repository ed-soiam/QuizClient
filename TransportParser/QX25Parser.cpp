#include "QX25Parser.h"

/**
 * @brief crc16 - подсчет CRC16 для блока данных.
 * @param buf - блок данных
 * @return - вычисленная CRC16
 */

uint16_t QX25Parser::crc16(const uint8_t * data, unsigned char len)
{

    uint16_t i;
    uint16_t crc = 0x0000;
    uint8_t crc_data;
    //не знаю, как это должно работать, не мое
    while (len--)
    {
        crc_data = *data++;

        for (i = 0; i < 8; i++)
        {
            if ( crc & 0x8000 )
            {
                crc <<= 1;
                if (crc_data & 0x0001)
                    crc |= 0x0001;
                crc ^= 0x1021;
            }
            else
            {
                crc <<= 1;
                if (crc_data & 0x0001)
                    crc |= 0x0001;
            }
            crc_data >>= 1;
        }
    }

    return crc;

}

QX25Parser::QX25Parser(QObject *parent) : QTransportParser(parent)
{
    staf_flag = false;
}


QByteArray QX25Parser::parse(const QByteArray & data)
{
    QByteArray result;
    uint16_t crc;
    result.reserve(128);
    in.append(data);
    unsigned char byte;
    if (in.size() <= X25_BYTE_SIZE_OFFSET)
        return result;//даже не анализируем, пока мало данных

    for (int i = 0; i < in.size(); i++)
    {
        byte = (unsigned char)in[i];
        if ((byte != X25_START_BYTE) && result.isEmpty())
        {
            in = in.remove(0,i + 1);
            i = -1;
            continue;//ошибка во время начала последовательности приняли неверный айт начала последоательности. игнорируем
        }

        if (byte == X25_START_BYTE && !staf_flag && !result.isEmpty())
            result.clear();


        if ((byte == X25_STUFF_BYTE) && (!staf_flag))//если мы получили первый стафсимвол
        {
            staf_flag = true;
            continue;
        }

        if (staf_flag)//сброс стаф-флага и запись символа, следующего за стаф-символом, в буффер
        {
            if (byte != X25_STUFF_BYTE && byte != X25_START_BYTE)
            {//ситуация, когда за стафф байтом не следует верный байт подмены

            }
            staf_flag = false;
        }

        result.push_back(byte);
        //в in[NetroMessage::BYTE_SIZE_OFFSET] хранится длина пакета
        if ((result.size() > X25_BYTE_SIZE_OFFSET) && result.size() > X25_BYTE_SIZE_OFFSET + in[X25_BYTE_SIZE_OFFSET] + (int)sizeof(crc))//получили весь пакет
        {
            in = in.remove(0,i + 1);
            crc = crc16((const uint8_t *)(result.data() + 1),result.size() - sizeof(crc) - 1);
            if (crc != ((uint8_t)result.at(result.size() - 2) | (((uint16_t)((uint8_t)result.at(result.size() - 1))) << 8)))
            {//не совпало crc, ищем дальше новое сообщение
                i = -1;
                continue;
            }
            return result;
        }
   }
   return QByteArray();
}


QByteArray QX25Parser::create(const QByteArray & data)
{
    QByteArray res;
    res.reserve(data.size() + 10);
    if (data.isEmpty())
        return QByteArray();
    res.push_back(data.at(0));
    for (int i = 1; i < data.size(); i++)
    {
        switch (data.at(i))
        {
        case X25_START_BYTE:
        case X25_STUFF_BYTE:
            res.push_back(X25_STUFF_BYTE);
            break;
        default:
            break;
        }
        res.push_back(data.at(i));
    }
    return res;
}


void QX25Parser::reset()
{
    in.clear();
}
