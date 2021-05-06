#ifndef XVCAMPROPOBJ_H
#define XVCAMPROPOBJ_H

#include "xVTypes.h"
#include "xVGenVisPropObj.h"
#include <QThread>
#include <pylon/InstantCamera.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

// Namespace for using pylon objects.
using namespace Pylon;

class xVCamPropObj;

//!
//! \brief The CPixelFormatAndAoiConfiguration class
//! handles the transfer of the parameter to the camera and informs about disconnect and grabbing finished
//!
class CPixelFormatAndAoiConfiguration : public QObject, public Pylon::CConfigurationEventHandler
{
Q_OBJECT
public:
    CPixelFormatAndAoiConfiguration(xVCamPropObj*);
    virtual void OnOpened( Pylon::CInstantCamera& camera) override;
    virtual void OnGrabStarted( CInstantCamera& camera) override;
    virtual void OnCameraDeviceRemoved( CInstantCamera& camera) override;
    virtual void OnGrabStopped( CInstantCamera& camera) override;

signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
    void CameraDeviceRemoved();
    void GrabbingStopped();
protected:
    xVCamPropObj *pCamPropRef = nullptr;
};

enum xVCameraThread_Event_Types
{
    CAMERA_FRAME = QEvent::User,
    CAMERA_AV_BRIGHTNESS = QEvent::User+1
};

class xVCameraThread_Event : public QEvent
{
public:
    xVCameraThread_Event(QImage *p):QEvent(QEvent::User)
    {
        pImg = p;
    }
    QImage *image(){return pImg;}
protected:
    QImage *pImg = NULL;
};

class xVCamThread:public QThread
{
    Q_OBJECT
public:
    xVCamThread(Pylon::CInstantCamera* pCam,xVCamPropObj*);
    virtual void run() override;
    void stop();
signals:
    void update();
protected:
    bool _abb = false;
    Pylon::CInstantCamera *pCamera = nullptr;
    xVCamPropObj* pCamPropObj = nullptr;
};

//!
//! \brief The xVCamPropObj class
//! is used to grab images from an pylon instant camera created with a xVCamObj.
//! You can grab single images, a specified number of images or a continous stream of images.
//! All acquistion settings are also part of this class.
//! The output is a vtkImage or vtkImageStack. In case of continous acquisition the vtkImage is updated for each frame.
//!
class xVCamPropObj:public xVGenVisPropObj
{
    Q_OBJECT
public:
    xVCamPropObj(const QString&);
    xVCamPropObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;
signals:
    void GrabbingStopped();
protected slots:
    void CameraDeviceRemoved();
    void updateImage();
    void paramModified(const QString& txt="") override;
protected:
    void retrieveValuesAndLimitsFromCamera();
    CPixelFormatAndAoiConfiguration *pCamConfig = NULL;
    xVCamThread *pCamThread = nullptr;
};

#endif // XVCAMPROPOBJ_H
