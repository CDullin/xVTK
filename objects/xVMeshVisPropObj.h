#ifndef XVMESHVISPROPOBJ_H
#define XVMESHVISPROPOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisPropObj.h"
#include <vtkOpenGLPolyDataMapper.h>

class xVMeshVisPropObj:public xVGenVisPropObj
{
Q_OBJECT
public:
    xVMeshVisPropObj(const QString& txt="");
    xVMeshVisPropObj(QDataStream &d);
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
    virtual void reset() override;
public slots:
    virtual void paramModified(const QString& txt="") override;
protected:
    vtkProperty* backProp=nullptr;
    vtkActor* actor=nullptr;
};


#endif // XVMESHVISPROPOBJ_H
