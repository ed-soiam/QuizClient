#ifndef QTHREADOBJECT_H
#define QTHREADOBJECT_H

#include <QObject>
#include <QThread>

class QThreadObject : public QObject
{
    Q_OBJECT          //Обязательный макрос для сигналов и слотов
protected:            //Защищенный
    bool f_doStop;    //остановить
    bool f_stopped;   //остановлен
    QThread * thread;    
    bool canContinue() const{return !f_doStop;} //Для вывода f_doStop
public:
    typedef enum {
        THROW_ERROR_NO_CONNECT    //бросок
    } THROW_ERROR;
    explicit QThreadObject();     //Конструктор
    virtual ~QThreadObject();
    void start();                   //функция запуск

signals:
    void finished();     //Завершение
public slots:
    virtual void process() = 0;
private slots:
    void whenFinished();  //При завершении
};

#endif // QTHREADOBJECT_H
