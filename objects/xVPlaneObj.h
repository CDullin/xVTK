#ifndef XVPLANEOBJ_H
#define XVPLANEOBJ_H

#include "xVTypes.h"
#include "xVGenVisPropObj.h"
#include "vtkImagePlaneWidget.h"
#include "vtkClipVolume.h"
#include "vtkCommand.h"
#include "vtkPlane.h"
#include "vtkImageReslice.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkPlaneCollection.h"

namespace {
class xVPlaneCallback : public vtkCommand
{
public:
  static xVPlaneCallback* New()
  {
    return new xVPlaneCallback;
  }
  void On(){_enabled=true;}
  void Off(){_enabled=false;}
  void setInverted(bool b){_inverted=b;}
  vtkVolume *m_volume=nullptr;
  vtkPlane *pPlane=nullptr;
  bool _enabled=true;
  bool _inverted=false;
  void SetVolume(vtkVolume* p){m_volume=p;}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {

    vtkSmartPointer<vtkImagePlaneWidget> planeWidget =
        dynamic_cast<vtkImagePlaneWidget*>(caller);

    if (planeWidget)
    {
        if (!pPlane)
            pPlane=vtkPlane::New();
        double o[4],n[4];
        planeWidget->GetOrigin(o);
        planeWidget->GetNormal(n);
        if (_inverted)
        {
            n[0]=-n[0];
            n[1]=-n[1];
            n[2]=-n[2];
            n[3]=-n[3];
        }
        pPlane->SetOrigin(o);
        pPlane->SetNormal(n);

        if (m_volume && m_volume->GetMapper())
        {
            vtkPlaneCollection *pCollection=m_volume->GetMapper()->GetClippingPlanes();
            bool _found=false;
            if (pCollection)
            {
                vtkPlane *pCurrentPlane=nullptr;
                pCollection->InitTraversal();
                do {
                    pCurrentPlane=pCollection->GetNextItem();
                    _found|=pCurrentPlane==pPlane;
                } while (pCurrentPlane);
            }
            if (_enabled && !_found) m_volume->GetMapper()->AddClippingPlane(pPlane);
            if (!_enabled && _found) m_volume->GetMapper()->RemoveClippingPlane(pPlane);
            m_volume->Update();
        }
    }
  }

  xVPlaneCallback()
  {
  }
};
} // namespace
class xVPlaneObj:public xVGenVisPropObj
{
    Q_OBJECT
public:
    xVPlaneObj(const QString &txt="");
    xVPlaneObj(QDataStream &d);
    virtual void run() override;
    virtual void paramModified(const QString& txt="") override;
protected:
    vtkImagePlaneWidget *pPlaneWdgt=nullptr;
    vtkPlane *pPlane = nullptr;
    vtkClipVolume *pClipVolume = nullptr;
    xVPlaneCallback *pCallBack=nullptr;
};

typedef vtkImagePlaneWidget* vtkImagePlaneWidgetPtr;
Q_DECLARE_METATYPE(vtkImagePlaneWidgetPtr);

#endif // XVPLANEOBJ_H
