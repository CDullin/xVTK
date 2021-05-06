#ifndef XVARDUINOTHREAD_H
#define XVARDUINOTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class xVArduinoThread : public QThread
{
    Q_OBJECT

    enum ANESTHESIA_SIGNALS
    {
        AS_CONNECTION_REQUEST           = 0x01,
        AS_ARDUINO_CONNECTED_TO_PC      = 0x02,
        AS_REQUEST_INIT                 = 0x03,
        AS_SYSTEM_INITILIZED            = 0x04,
        AS_SET_ANGLE                    = 0x05,
        AS_ANGLE_RECEIVED               = 0x06,
        AS_GET_CURRENT_ANGLE            = 0x07,
        AS_CURRENT_ANGLE                = 0x08,
        AS_ARDUINO_ERROR                = 0x09
    };

public:
    explicit xVArduinoThread(QObject *parent = nullptr);
    ~xVArduinoThread();

    void transaction(const QString &portName, int waitTimeout, const QString &request);

signals:
    void response(const QString &s,const bool& b=false);
    void error(const QString &s,const bool& b=true);
    void timeout(const QString &s,const bool& b=false);

private:
    void run() override;

    QString m_portName;
    QString m_request;
    int m_waitTimeout = 0;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit = false;
};

#endif // XVARDUINOTHREAD_H
