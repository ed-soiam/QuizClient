#include "QSimpleCommandParser.h"

QSimpleCommandParser::QSimpleCommandParser(QObject *parent) : QTransportParser(parent)
{

}


QByteArray QSimpleCommandParser::parse(const QByteArray & data)
{
    in.append(data);
    int pos = in.indexOf(';'); //Возвращает индексную позицию первого вхождения байтового массива ba в этом массиве байтов, ища вперед от позиции индекса. Возвращает -1, если ba не удалось найти.
    if (pos == 0)//delete empty cmd/Команда пустая
        in = in.right(in.size() - 1);
    //in.right(int len)Возвращает массив байтов, содержащий самые правые байты этого байтового массива.
    //in.size()Возвращает количество байтов в этом массиве байтов
    if (pos == -1 || pos == 0)
        //Возвращает -1, если вхождение не удалось найти.
        return QByteArray(); //Не понимаю, что это!
    QByteArray result = in.left(pos);
    in = in.right(in.size() - pos - 1);
    return result;
}


QByteArray QSimpleCommandParser::create(const QByteArray & data)
{
    QByteArray result;
    result.reserve(data.size() + 1);
    result.append(data);
    result.append(';');
    return result;
}


void QSimpleCommandParser::reset()
{
    in.clear();
}
