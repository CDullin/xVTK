#ifndef XVDEFUZZIFICATIONOBJ_H
#define XVDEFUZZIFICATIONOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVDefuzzificationObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVDefuzzificationObj(const QString&);
    xVDefuzzificationObj(QDataStream &d);
    virtual void run() override;

protected:
    void paramModified(const QString&) override;
    void generateShape();
};


#endif // XVDEFUZZIFICATIONOBJ_H
