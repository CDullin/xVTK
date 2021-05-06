#ifndef XVEXPORTOBJ_H
#define XVEXPORTOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisObj.h"

class xVExportObj:public xVGenVisObj
{
    Q_OBJECT
public:
    xVExportObj(const QString& txt="");
    xVExportObj(QDataStream &);
    virtual void run() override;
protected:
    bool exportImage(vtkImageDataPtr);
    bool exportVolume(vtkVolumePtr);
    bool exportMesh(vtkPolyDataMapperPtr);

    virtual void paramModified(const QString&) override;
    xVProgressObserver* pProgObserver = nullptr;
};

#endif // XVEXPORTOBJ_H
