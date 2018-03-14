#ifndef QSHMANAGERINFO_H
#define QSHMANAGERINFO_H
#include <QApplication>
#include <QMutex>
//#include "guiprop.h"
#include "qthreadobject.h"
#include "TransportParser/QJSONParser.h"
//#include "tasks/qjsontask.h"
#include "QThreadSocket.h"
class QSHManagerInfo : public QThreadObject
{
    Q_OBJECT
public:
    explicit QSHManagerInfo(QObject *parent = 0, const QString & path = QString());
    virtual ~QSHManagerInfo();
    //отправка JSON event в сокет с возможностью добавления доп. информации в additional_json_fields(например QString("\"ip\":\"192.168.0.11\""))
    void sendEvent(const QString & event,const QString & additional_json_fields = QString());
    bool isGSMInited() const {return _gsm_inited;}
    //перевод уровня gsm из дб в %
    static QGuiProps::EStatePropGSMQuality gsmLevel(int dbm_level);
signals:
    void writeToSocket(QByteArray);
    void WPSStarted(int time_sec);
    void WPSStopped();
    void setConnected(bool);
private slots:
    void parseMessage(const QByteArray & data);
    void logError(const QString & err);
private:
    virtual void process();
    void execTasks();
    void parseJSONAnswer(const QVariantMap & answer);
    QThreadSocket socket;

    //задачи для менеджера, парсер ожидания текущего ответа и время таймаута
    QList<QJSONTask> _tasks;
    QJSONTask current_task;

    QMutex local_mutex;
    int _label = 1;
    bool _gsm_inited = false;
};

#endif // QSHMANAGERINFO_H
