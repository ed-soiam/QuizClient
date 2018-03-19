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
    emit destroyed(this);     //void QObject::destroyed(QObject *obj = Q_NULLPTR) Этот сигнал испускается непосредственно перед уничтожением объекта obj и не может быть заблокирован.
                              //Все дети объектов уничтожаются сразу же после выхода этого сигнала.

    thread->deleteLater();
    while (!f_stopped)
        QApplication::processEvents(QEventLoop::AllEvents, 1); //Обработает все ожидающие события для вызывающего потока в соответствии с указанными флажками, пока не будет больше событий для обработки.
}

void QThreadObject::start()
{
    f_doStop = false;
    f_stopped = false;    
    thread = new QThread();
    connect( thread, SIGNAL(started()), this, SLOT(process())); //Это частный сигнал QThread. Испускается, когда он запускается, перед вызовом функции run ()
    //Этот сигнал испускается из связанного потока, когда он запускается, перед вызовом функции run ().
    //process() - это виртуальный слот реализованный в наследнике
    connect( this, SIGNAL (finished()), this, SLOT(whenFinished()), Qt::QueuedConnection); //Сигнал от наследника

    moveToThread(thread);   //Перемещает обработку событий в поток thread
    thread -> start();      //Начинает выполнение потока, вызывая run ().
}


void QThreadObject::whenFinished()
{
    f_doStop = false;
    f_stopped = true;    
    thread -> exit();  //Сообщает, что цикл событий потока завершится с кодом возврата. После вызова этой функции поток покидает цикл событий
    //thread -> deleteLater();  //Объект будет удален, когда управление вернется в цикл событий.
}
