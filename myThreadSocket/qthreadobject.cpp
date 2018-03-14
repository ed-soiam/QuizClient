#include "qthreadobject.h"
#include <QApplication>

QThreadObject::QThreadObject() : QObject(nullptr)
{
    f_doStop = false;
    f_stopped = true;
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
    f_stopped = false;
    f_doStop = false;
    thread = new QThread();
    connect( thread, SIGNAL(started()), this, SLOT(process()));
    connect( this, SIGNAL (finished()), this, SLOT(whenFinished()), Qt::QueuedConnection);
    moveToThread(thread);
    thread -> start();
}


void QThreadObject::whenFinished()
{
    f_stopped = true;
    f_doStop = false;
    thread -> exit();
    thread -> deleteLater();
}
