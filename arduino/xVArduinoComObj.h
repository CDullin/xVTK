#ifndef XVARDUINOCOMOBJ_H
#define XVARDUINOCOMOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include <QThread>
#include <QEvent>
#include <QMutex>

enum ARDUINO_COM_CODE
{
    SEND_MSG        = 0x01,
    RECEIVE_MSG     = 0x02,
    INITIALIZE      = 0x03,
    STATUS_REQ      = 0x04,
    STATUS_ANSWER   = 0x05,
    SEND_INT        = 0x06,
    RECEIVE_INT     = 0x07,
    SEND_DOUBLE     = 0x08,
    RECEIVE_DOUBLE  = 0x09
};

class xVArduinoComThread_Event:public QEvent
{
public:
    enum xVArduinoComThread_Event_Type
    {
        MSG_RECEIVED = 0x01,
        THREAD_STARTED = 0x02,
        THREAD_TERMINATED = 0x03,
        ERROR = 0x04
    };
    xVArduinoComThread_Event(const xVArduinoComThread_Event_Type& t,void *pData=nullptr);
    xVArduinoComThread_Event_Type type(){return _type;}
    void* data(){return pData;}
protected:
    xVArduinoComThread_Event_Type _type=MSG_RECEIVED;
    void *pData=nullptr;
};

class xVArduinoComThread:public QThread
{
public:
    xVArduinoComThread(xSerialPortPtr pPort);

    virtual void run() override;
    void stop();
    void sendMsg(const QString&);
    QString receivedMsg();
    int receivedMsgCount();

protected:
    bool _abb = false;
    xSerialPortPtr pSerial = nullptr;
    QStringList _sendMsgLst;
    QStringList _receivedMsgLst;
    QMutex _sendMutex,_receiveMutex;
};

class xVArduinoComObj:public xVObj_Basics
{
    Q_OBJECT

public:
    xVArduinoComObj(const QString&);
    xVArduinoComObj(QDataStream& d);
    virtual void run() override;
    virtual void reset() override;
protected:
    QString encode(const ARDUINO_COM_CODE& code,const xPROP_TYPE& prop);
    void decode(const QString&);
    void generateShape();
};
#endif // XVARDUINOCOMOBJ_H
