#ifndef XVSTARTOBJ_H
#define XVSTARTOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVStartObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVStartObj(const QString& txt);
    xVStartObj(QDataStream& d);

    virtual void run() override;
    virtual void reset() override;

protected:
    void generateShape();
};

#endif // XVSTARTOBJ_H
