#ifndef XVBOXOBJ_H
#define XVBOXOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisPropObj.h"
#include "vtkBoxWidget.h"
#include "vtkCommand.h"
#include "vtkNew.h"
#include "vtkBoxRepresentation.h"
#include "vtkTransform.h"
#include "vtkPlanes.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkPlaneCollection.h"
#include "vtkPropCollection.h"

namespace {
class xVBoxCallback : public vtkCommand
{
public:
  static xVBoxCallback* New()
  {
    return new xVBoxCallback;
  }
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {

    vtkSmartPointer<vtkBoxWidget> boxWidget =
        dynamic_cast<vtkBoxWidget*>(caller);

    if (boxWidget)
    {
        vtkPlanes *pPlanes=vtkPlanes::New();
        vtkTransform *pTransfrom=vtkTransform::New();
        boxWidget->GetTransform(pTransfrom);
        boxWidget->GetPlanes(pPlanes);
        vtkPropCollection* pPropCollection = vtkPropCollection::New();
        boxWidget->GetProp3D()->GetVolumes(pPropCollection);
        if (pPropCollection)
        {
            pPropCollection->InitTraversal();
            vtkVolume *pVolume=dynamic_cast<vtkVolume*>(pPropCollection->GetNextProp());
            if (pVolume && pVolume->GetMapper())
            {
                //pVolume->GetMapper()->SetClippingPlanes(pPlanes);
                pVolume->SetUserTransform(pTransfrom);
                pVolume->Update();
            }
        }
    }
  }

  xVBoxCallback()
  {
  }
};
} // namespace

class xVBoxObj:public xVGenVisPropObj
{
Q_OBJECT
public:
    xVBoxObj(const QString& txt="");
    xVBoxObj(QDataStream &d);
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
    virtual void reset() override;
public slots:
    virtual void paramModified(const QString& txt="") override;
protected:
    vtkBoxWidget* pBoxWdgt=nullptr;
    xVBoxCallback* pBoxCallBack=nullptr;
};


#endif // XVBOXOBJ_H
