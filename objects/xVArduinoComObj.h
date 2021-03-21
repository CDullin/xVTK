#ifndef XVARDUINOCOMOBJ_H
#define XVARDUINOCOMOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVArduinoComObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVArduinoComObj(const QString&);
    xVArduinoComObj(QDataStream& d);
    virtual void run() override;
protected:
    void generateShape();
};
#endif // XVARDUINOCOMOBJ_H
