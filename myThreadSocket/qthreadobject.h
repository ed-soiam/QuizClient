#ifndef QTHREADOBJECT_H
#define QTHREADOBJECT_H

#include <QObject>
#include <QThread>
//#include "LogItem.h"

class QThreadObject : public QObject
{
    Q_OBJECT
protected:
    bool f_doStop;
    bool f_stopped;
    QThread * thread;
    LogBuffer * log;
    bool canContinue() const{return !f_doStop;}
public:
    typedef enum {
        THROW_ERROR_NO_CONNECT
    } THROW_ERROR;
    explicit QThreadObject();
    virtual ~QThreadObject();
    void start();
    inline void setLog(LogBuffer * log){this -> log = log;}
signals:
    void finished();
public slots:
    virtual void process() = 0;
private slots:
    void whenFinished();
};

#endif // QTHREADOBJECT_H
