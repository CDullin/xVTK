#ifndef XVPOLYOBJ_H
#define XVPOLYOBJ_H

#include "xVObjects.h"
#include "xVTypes.h"
#include "xVGenImpObj.h"

#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTimerLog.h>
#include <vtksys/SystemTools.hxx>

#include <algorithm>
#include <array>
#include <random>
#include <string>

class xVPolyObj:public xVGenImpObj
{
public:
    xVPolyObj(const QString& txt="");
    xVPolyObj(QDataStream &d);
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
    virtual void reset() override;
public slots:
    virtual void paramModified(const QString& txt="") override;
protected:
    vtkPolyData* ReadPolyData(const char* fileName);
    vtkPolyData* polyData = nullptr;
    vtkPolyDataMapper* mapper = nullptr;
    xVProgressObserver* pProgObserver = nullptr;
};
#endif // XVPOLYOBJ_H
