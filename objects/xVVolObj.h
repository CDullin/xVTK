#ifndef XVVOLOBJ_H
#define XVVOLOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenImpObj.h"
#include "xVHisto.h"

#include <vtkRayCastImageDisplayHelper.h>

class xVHisto;

class xVVolObj:public xVGenImpObj
{
Q_OBJECT
public:
    xVVolObj(const QString& txt="");
    xVVolObj(QDataStream &d);
    vtkImageData* data(){return pDataPtr;}
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
    virtual void reset() override;
    xVHisto* histo(){return pHisto;}
protected:
    xVProgressObserver* pProgObserver = nullptr;
    xVHisto* pHisto = nullptr;
    vtkImageData* pDataPtr = nullptr;
    vtkImageReader *pDataImporter = nullptr;
};


#endif // XVVOLOBJ_H
