#ifndef XVENDOBJ_H
#define XVENDOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVEndObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVEndObj(const QString&);
    xVEndObj(QDataStream&);
    virtual void run() override;
protected:
    void generateShape();
};

#endif // XVENDOBJ_H
