#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVariantMap>
#include "qjsontask.h"

QJSONTask::QJSONTask(QObject *parent) : QTask(parent)
{

}

QJSONTask::QJSONTask(const QJSONTask & obj):QTask(obj) //Корстуктор копирования,obj это старый обьект,this указатель на новый объект, типа сахранить здесь
{
    *this = obj;
}

QJSONTask & QJSONTask::operator=(const QJSONTask & obj) //Оператор присвоения необходим в сулучае наличия указателей на объект
{
    QTask::operator =(obj);
    _answer_parser = obj._answer_parser;
    _label = obj._label;
    return *this;
}


void QJSONTask::setLabel(int value) //Установка метки
{
    _label = value;

    QJsonParseError json_error;
    //Класс QJsonParseError используется для сообщения об ошибках во время разбора JSON.

    QJsonDocument json_doc = QJsonDocument::fromJson(_data,&json_error);
    //Класс QJsonDocument обеспечивает способ чтения и записи документов JSON.
    //Возвращает действительный (не нулевой) QJsonDocument, если синтаксический анализ завершается успешно.
    //Если это не удается, возвращаемый документ будет пустым, а дополнительная переменная ошибки будет содержать дополнительные сведения об ошибке.

    if (json_error.error != QJsonParseError::NoError)
        return;//не json формат, ничего не поделать

    QVariantMap command_map = json_doc.object().toVariantMap();
    //Синоним QMap <QString, QVariant>.
    //Преобразует этот объект в QVariantMap. Возвращает созданную карту.

    if (command_map["label"].isValid()) //Возвращает true, если тип хранилища этого варианта не QMetaType :: UnknownType; иначе возвращает false.
        command_map.take("label"); //Удаляет элемент с помощью ключевого ключа с карты и возвращает связанное с ним значение.
    //label - это ключ в команде

    command_map.insert("label",_label);//Вставляет новый элемент с ключевым ключом и значением значения.


    QJsonObject json_obj = QJsonObject::fromVariantMap(command_map);
    //Преобразует variant map в объект QJsonObject.
    json_doc.setObject(json_obj);
    //Устанавливает объект как основной объект этого документа.
    _data = json_doc.toJson(QJsonDocument::Compact);
    //Преобразует QJsonDocument в документ JSON, закодированный в UTF-8, в предоставленном формате.
}


void QJSONTask::setTaskData(const QByteArray & data)
{
    QTask::setTaskData(data);
}
