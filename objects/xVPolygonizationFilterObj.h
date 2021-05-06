#ifndef XVPOLYGONIZATIONFILTEROBJ_H
#define XVPOLYGONIZATIONFILTEROBJ_H

#include "xVTypes.h"
#include "xVGenFilterObj.h"
#include "vtkMarchingCubes.h"
#include "vtkImageData.h"

class xVPolygonizationFilterObj:public xVGenFilterObj
{
    Q_OBJECT
public:
    xVPolygonizationFilterObj(const QString& txt="");
    xVPolygonizationFilterObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;
protected:
    vtkMarchingCubes *pMarchingCubes = nullptr;
    vtkPolyDataMapper *pMapper = nullptr;
    xVProgressObserver* pProgObserver = nullptr;
};


#endif // XVPOLYGONIZATIONFILTEROBJ_H
