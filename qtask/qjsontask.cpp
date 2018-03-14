#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVariantMap>
#include "qjsontask.h"

QJSONTask::QJSONTask(QObject *parent) : QTask(parent)
{

}

QJSONTask::QJSONTask(const QJSONTask & obj):QTask(obj)
{
    *this = obj;
}

QJSONTask & QJSONTask::operator=(const QJSONTask & obj)
{
    QTask::operator =(obj);
    _answer_parser = obj._answer_parser;
    return *this;
}


void QJSONTask::setLabel(int value)
{
    _label = value;

    QJsonParseError json_error;
    QJsonDocument json_doc = QJsonDocument::fromJson(_data,&json_error);
    if (json_error.error != QJsonParseError::NoError)
        return;//не json формат, ничего не поделать
    QVariantMap command_map = json_doc.object().toVariantMap();
    if (command_map["label"].isValid())
        command_map.take("label");
    command_map.insert("label",_label);
    QJsonObject json_obj = QJsonObject::fromVariantMap(command_map);
    json_doc.setObject(json_obj);
    _data = json_doc.toJson(QJsonDocument::Compact);
}


void QJSONTask::setTaskData(const QByteArray & data)
{
    QTask::setTaskData(data);
    setLabel(_label);
}
