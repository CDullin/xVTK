#ifndef XVVOLUMEVISPROPOBJ_H
#define XVVOLUMEVISPROPOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisPropObj.h"

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkImageReader.h>

class xVVolumeVisPropObj:public xVGenVisPropObj
{
Q_OBJECT
public:
    xVVolumeVisPropObj(const QString& txt="");
    xVVolumeVisPropObj(QDataStream &d);
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
    virtual void reset() override;
public slots:
    virtual void paramModified(const QString& txt="") override;
protected slots:
    virtual void updateTimerTimeOut() override;
protected:
    vtkVolumeProperty* volumeProperty=nullptr;
    vtkFixedPointVolumeRayCastMapper* volumeMapper=nullptr;
    vtkVolume *volume=nullptr;
};

#endif // XVVOLUMEVISPROPOBJ_H
