#include "qthreadobject.h"
#include <QApplication>

QThreadObject::QThreadObject() : QObject(nullptr)
{
    f_doStop = false;      //остановить
    f_stopped = true;      //остановлен
}

QThreadObject::~QThreadObject()
{
    f_doStop = true;
    emit destroyed(this);
    thread -> deleteLater();
    while (!f_stopped)
        QApplication::processEvents(QEventLoop::AllEvents, 1);
}

void QThreadObject::start()
{
    f_doStop = false;
    f_stopped = false;    
    thread = new QThread();
    connect( thread, SIGNAL(started()), this, SLOT(process()));
    //Этот сигнал испускается из связанного потока, когда он запускается, перед вызовом функции run ().
    connect( this, SIGNAL (finished()), this, SLOT(whenFinished()), Qt::QueuedConnection); //При завершении
    moveToThread(thread); //Перемещает обработку событий в поток thread
    thread -> start();      //Начинает выполнение потока, вызывая run ().
}


void QThreadObject::whenFinished()
{
    f_doStop = false;
    f_stopped = true;    
    thread -> exit();
    thread -> deleteLater();
}
