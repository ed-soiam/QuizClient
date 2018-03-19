#ifndef QTASK_H
#define QTASK_H
#include <QDateTime>
#include <QObject>
#include <QElapsedTimer>

class QTask : public QObject
{
    Q_OBJECT
public:
    explicit QTask(QObject *parent = 0);   //Конструктор без аргументов
    QTask(const QTask & obj);               //Конструктор копирования
    QTask & operator =(const QTask & obj);  //Оператор присваивания
    virtual void setTaskData(const QByteArray & data) {_data = data;}  //Инициализация данных для передачи
    QByteArray taskData() const {return _data;}                         //Извлечение данных из класса/вывод
    //условие проверки активности задачи по внешней переменной
    void setConditionPointer(bool * pointer){_pre_check = pointer;}

    void setInfinity(bool value){_infinity = value;}  //Устанавливает бесконечность исполнения
    bool infinity() const{return _infinity;}

    void setPeriod(int value_ms){_period_ms = value_ms;}
    int period() const{return _period_ms;}

    void setTimeout(int value_ms){_timeout_ms = value_ms;}
    int timeout() const{return _timeout_ms;}

    void execute() {_last_exec = _task_timer.elapsed();}
    bool isReady() const;   //Готов

    bool isTimeout() const {return _task_timer.elapsed() - _last_exec > _timeout_ms ? true :false;}
    /*Назначить функцию пост обработки задачи в случае успешного выполнения и ошибки*/
    void setPostOkFunction(void (*func)(void *), void * context = nullptr);
    void setPostErrorFunction(void (*func)(void *,int), void * context = nullptr);

    void setOk();
    void setError(int error);

    void setRetryCount(int value){_retries = value;} //повторы
    int retryCount() const{return _retries;}         //повторы
protected:

    QByteArray _data;       //данные для передачи

    bool * _pre_check = 0;   //проверка условия, если указатель активен
    bool _infinity = false;          //бесконечность исполнения

    qint64 _last_exec = 0;    //время последнего запуска
    int _period_ms = 0;           //задержка между запусками
    int _timeout_ms = 1000;//таймаут ожидания ответа от задачи

    void * _ok_context, * _error_context;
    void (*_funcPostOk)(void *);
    void (*_funcPostError)(void *,int);
    int _retries;//количество повторов при ошибке отработки задачи
private:
    QElapsedTimer _task_timer;
};

#endif // QTASK_H
