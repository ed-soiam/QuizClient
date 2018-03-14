#include "qtask.h"

QTask::QTask(QObject *parent) : QObject(parent)
{
    _task_timer.start();
    _ok_context = 0;
    _error_context = 0;
    _funcPostOk = nullptr;      //Сообщение Ок
    _funcPostError = nullptr;   //Сообщение об ошибке
    _retries = 0;               //количество повторов при ошибке отработки задачи
}


QTask::QTask(const QTask & obj) : QObject(obj.parent()) //Не понятно
{
    *this = obj;      //Не понятно
}


QTask & QTask::operator =(const QTask & obj)
{
    _data = obj._data;
    _pre_check = obj._pre_check;
    _infinity = obj._infinity;
    _last_exec = obj._last_exec;
    _period_ms = obj._period_ms;
    _timeout_ms = obj._timeout_ms;
    _ok_context = obj._ok_context;
    _error_context = obj._error_context;
    _funcPostOk = obj._funcPostOk;
    _funcPostError = obj._funcPostError;
    _retries = obj._retries;
    _task_timer = obj._task_timer;
    return *this;
}

bool QTask::isReady() const   //Готов
{
    if (_pre_check && *_pre_check == false)
        return false;
    if (_last_exec + _period_ms <= _task_timer.elapsed() || !_last_exec) // _last_exec время последнего запуска
        return true;
    return false;
}

void QTask::setPostOkFunction(void (*func)(void *), void * context)
{
    _funcPostOk = func;
    _ok_context = context;
}

void QTask::setPostErrorFunction(void (*func)(void *,int), void * context)
{
    _funcPostError = func;
    _error_context = context;
}


void QTask::setOk()
{
    if(_funcPostOk)
        _funcPostOk(_ok_context);
}


void QTask::setError(int error)
{
    if (_retries > 0)
        _retries--;

    if(!_retries && _funcPostError)
        _funcPostError(_ok_context,error);

}
