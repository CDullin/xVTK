#ifndef XVMATHOBJ_H
#define XVMATHOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVMathObj : public xVObj_Basics
{
    Q_OBJECT
public:
    xVMathObj(const QString&);
    xVMathObj(QDataStream& d);

    virtual void run() override;
    virtual void save(QDataStream& d,bool _explicit=false) override;
    virtual void reset() override;

protected:
    virtual void generateShape();
};

#endif // XVMATHOBJ_H
