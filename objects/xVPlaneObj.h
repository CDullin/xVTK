#ifndef XVPLANEOBJ_H
#define XVPLANEOBJ_H

#include "xVTypes.h"
#include "xVGenVisPropObj.h"
#include "vtkPlane.h"
#include "vtkActor.h"
#include "vtkClipVolume.h"

class xVPlaneObj:public xVGenVisPropObj
{
    Q_OBJECT
public:
    xVPlaneObj(const QString &txt="");
    xVPlaneObj(QDataStream &d);
    virtual void run() override;
    virtual void paramModified(const QString& txt="") override;
protected:
    vtkPlane *pPlaneVisObj = nullptr;
    vtkActor *pPlaneActor = nullptr;
    vtkClipVolume *pClipPlane = nullptr;
};

#endif // XVPLANEOBJ_H
