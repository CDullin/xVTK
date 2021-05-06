#include "xVArduinoThread.h"
#include <QSerialPort>
#include <QTime>

xVArduinoThread::xVArduinoThread(QObject *parent) :
    QThread(parent)
{
}

xVArduinoThread::~xVArduinoThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

void xVArduinoThread::transaction(const QString &portName, int waitTimeout, const QString &request)
{
    const QMutexLocker locker(&m_mutex);
    m_portName = portName;
    m_waitTimeout = waitTimeout;
    m_request = request;
    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
}

void xVArduinoThread::run()
{
    bool currentPortNameChanged = false;

    m_mutex.lock();
    QString currentPortName;
    if (currentPortName != m_portName) {
        currentPortName = m_portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = m_waitTimeout;
    QString currentRequest = m_request;
    m_mutex.unlock();
    QSerialPort serial;

    if (currentPortName.isEmpty()) {
        emit error(tr("control unit: No port name specified"));
        return;
    }

    while (!m_quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("control unit: Can't open %1, error code %2")
                           .arg(m_portName).arg(serial.error()));
                return;
            }
        }
        // write request
        const QByteArray requestData = currentRequest.toUtf8();
        serial.write(requestData);
        if (serial.waitForBytesWritten(m_waitTimeout)) {
            // read response
            if (serial.waitForReadyRead(currentWaitTimeout)) {
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                QString responseStr = QString::fromUtf8(responseData);
                emit response(responseStr);
            } else {
                emit timeout(tr("control unit: Wait read response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }
        } else {
            emit timeout(tr("control unit: Wait write request timeout %1")
                         .arg(QTime::currentTime().toString()));
        }
        m_mutex.lock();
        m_cond.wait(&m_mutex);
        if (currentPortName != m_portName) {
            currentPortName = m_portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = m_waitTimeout;
        currentRequest = m_request;
        m_mutex.unlock();
    }
}
