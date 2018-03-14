#ifndef QTHREADOBJECT_H
#define QTHREADOBJECT_H

#include <QObject>
#include <QThread>
//#include "LogItem.h"

class QThreadObject : public QObject   //Пока непонятно, для чего нежен этот класс
{
    Q_OBJECT
protected:            //Защищенный
    bool f_doStop;    //остановить
    bool f_stopped;   //остановлен
    QThread * thread;
    //LogBuffer * log;
    bool canContinue() const{return !f_doStop;} //Для вывода f_doStop
public:
    typedef enum {
        THROW_ERROR_NO_CONNECT    //бросок
    } THROW_ERROR;
    explicit QThreadObject();     //Конструктор
    virtual ~QThreadObject();
    void start();                   //функция запуск
    //inline void setLog(LogBuffer * log){this -> log = log;}
signals:
    void finished();     //При завершении
public slots:
    virtual void process() = 0;
private slots:
    void whenFinished();  //При завершении
};

#endif // QTHREADOBJECT_H
