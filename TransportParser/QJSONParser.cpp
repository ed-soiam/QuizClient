#include "QJSONParser.h"
#include <QJsonParseError>



QJSONParser::QJSONParser(QObject *parent) : QTransportParser(parent)
{

}


QJSONParser::~QJSONParser()
{

}


QByteArray QJSONParser::parse(const QByteArray & data)
{
    int i;
    in.append(data);
    QByteArray ba;

    while (1)                   //Бесконечный цикл, пока из него не выйти break или return
    {
        if (in.isEmpty())
            break;

        //препарсер json - выделение последовательности {...}
        bool json_caught = false; //Обработка следующего пакета начата
        int bracers = 0;
        for (i = 0; i < in.size();i++) //Перебор по байтам
        {
            ba.clear();
            ba.append(in[i]);

            if (in[i] == '{')
            {
                if (i && in[i - 1] == '}')//упс!!! не споймали прошлый пакет, откидываем его.
                {
                    in = in.remove(0,i); //Массив усекается c позиции 0 до позиции i
                    i = -1;
                    bracers = 0;
                    continue;
                }
                bracers++;
                json_caught = true;
            }
            else if (in[i] == '}')
            {
                bracers--;
            }

            if (!bracers && json_caught) //Была обнаружена последовательность {... }. Цикл останавливается
                break;
        }
        if (i == in.size())//не обнаружена последовательность {... }
            break;
        QByteArray res(in.data(),i + 1); //Создатеся массив байтов. data() возвращает указатель на данные, хранящиеся в массиве байтов.
        in = in.remove(0,i + 1); //Удаляются байты с позиции 0 до i+1
        return res;
    }
    return QByteArray(); //Конструктор возвращает пустой массив байтов.
}


void QJSONParser::reset()
{
    in.clear();
}
