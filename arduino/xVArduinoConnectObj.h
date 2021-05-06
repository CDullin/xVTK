#ifndef XVARDUINOCONNECTOBJ_H
#define XVARDUINOCONNECTOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVArduinoConnectObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVArduinoConnectObj(const QString&);
    xVArduinoConnectObj(QDataStream& d);
    virtual void run() override;
    virtual void reset() override;
protected:
    void generateShape();
};

#endif // XVARDUINOCONNECTOBJ_H
