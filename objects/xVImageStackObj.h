#ifndef XVIMAGESTACKOBJ_H
#define XVIMAGESTACKOBJ_H

#include "xVTypes.h"
#include "xVGenImpObj.h"

class xVImageStackObj:public xVGenImpObj
{
    Q_OBJECT
public:
    xVImageStackObj(const QString& txt="");
    xVImageStackObj(QDataStream &);
    virtual void run() override;
    virtual void reset() override;
};

#endif // XVIMAGESTACKOBJ_H
