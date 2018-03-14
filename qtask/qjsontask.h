#ifndef QJSONTASK_H
#define QJSONTASK_H

#include <QObject>
#include "qtask.h"
class QJSONTask : public QTask
{
    Q_OBJECT
public:
    typedef enum {
        JSON_ANSWER_NONE,
        JSON_ANSWER_GET_GSM_PARAMS
    }JSON_ANSWER_WAIT;
    explicit QJSONTask(QObject *parent = 0);
    QJSONTask(const QJSONTask & obj);
    QJSONTask & operator=(const QJSONTask & obj);
    void setAnswerParser(JSON_ANSWER_WAIT parser){_answer_parser = parser;}
    JSON_ANSWER_WAIT answerParser() const {return _answer_parser;}
    virtual void setTaskData(const QByteArray & data);
    void setLabel(int value);
    int label() const {return _label;}
private:
    JSON_ANSWER_WAIT _answer_parser = JSON_ANSWER_NONE;
    int _label = -1;
};

#endif // QJSONTASK_H
