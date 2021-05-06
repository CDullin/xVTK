#ifndef XVMORPHFILTEROBJ_H
#define XVMORPHFILTEROBJ_H

#include "xVTypes.h"
#include "xVGenFilterObj.h"

class xVMorphFilterObj:public xVGenFilterObj
{
    Q_OBJECT
public:
    xVMorphFilterObj(const QString& txt="");
    xVMorphFilterObj(QDataStream &);
    virtual void run() override;
};

#endif // XVMORPHFILTEROBJ_H
