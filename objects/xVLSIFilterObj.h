#ifndef XVLSIFILTEROBJ_H
#define XVLSIFILTEROBJ_H

#include "xVTypes.h"
#include "xVGenFilterObj.h"
#include "vtkImageMedian3D.h"

class xVLSIFilterObj:public xVGenFilterObj
{
    Q_OBJECT
public:
    xVLSIFilterObj(const QString& txt="");
    xVLSIFilterObj(QDataStream &d);
    virtual void run() override;

protected:
    vtkImageDataPtr pFilteredImage = nullptr;
    vtkThreadedImageAlgorithm *pAlgorithm = nullptr;
    xVProgressObserver* pProgObserver = nullptr;
};

#endif // XVLSIFILTEROBJ_H
