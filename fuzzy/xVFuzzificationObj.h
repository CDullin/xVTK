#ifndef XVFUZZIFICATIONOBJ_H
#define XVFUZZIFICATIONOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVFuzzificationObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVFuzzificationObj(const QString&);
    xVFuzzificationObj(QDataStream &d);
    virtual void run() override;

protected:
    void paramModified(const QString&) override;
    void generateShape();
};


#endif // XVFUZZIFICATIONOBJ_H
