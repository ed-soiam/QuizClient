#ifndef QJSONTASK_H
#define QJSONTASK_H

#include <QObject>
#include "qtask.h"
#include <QDebug>
class QJSONTask : public QTask //Используется для обработки label и необходимости получения ответа
{
    Q_OBJECT
public:
    typedef enum {
        JSON_ANSWER_NONE,              //ответа нет
        JSON_ANSWER_STANDARD,          //Ответ о выполнении команды
        JSON_ANSWER_STANDARD_REG       //Ответ на команду регистрации
    }JSON_ANSWER_WAIT;

    explicit QJSONTask(QObject *parent = 0); //Конструктор Нужно вспомнить QObject *parent = 0

    QJSONTask(const QJSONTask & obj);             //Конструктор копирования
                   // Скомпилируется если реализован конструктор X(const X&), и выдаст ошибку
                   // если определен только X(X&).
                   // Для того чтобы создать объект a компилятор создаст временный объект класса
                   // X и после этого с помощью конструктора копирования создаст объект a.
                   // Копирование временных объектов требует наличия const типа.

    QJSONTask & operator=(const QJSONTask & obj); //Оператор присвоения

    void setAnswerParser(JSON_ANSWER_WAIT parser){_answer_parser = parser;} //Задается тип парсера при постановке задачи

    JSON_ANSWER_WAIT answerParser() const {return _answer_parser;} //Возвращает тип парсера

    virtual void setTaskData(const QByteArray & data); //Инициалзиация _data в UTF-8

    void setLabel(int value);                          //Устанавливает метку

    int label() const {return _label;}                 //Возвращает метку
private:
    JSON_ANSWER_WAIT _answer_parser = JSON_ANSWER_NONE; //Текущий тим пответа для парсера задается при постановке задачи
    int _label = -1;
};

#endif // QJSONTASK_H
