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
    while (1)
    {
        if (in.isEmpty())
            break;

        //препарсер json - выделение последовательности {...}
        bool json_caught = false;
        int bracers = 0;
        for (i = 0; i < in.size();i++)
        {
            if (in[i] == '{')
            {
                if (i && in[i - 1] == '}')//упс!!! не споймали прошлый пакет, откидываем его
                {
                    in = in.remove(0,i);
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

            if (!bracers && json_caught)
                break;
        }
        if (i == in.size())//не обнаружена последовательность {... }
            break;
        QByteArray res(in.data(),i + 1);
        in = in.remove(0,i + 1);
        return res;
    }
    return QByteArray();
}


void QJSONParser::reset()
{
    in.clear();
}
