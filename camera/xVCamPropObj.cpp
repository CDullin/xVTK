#include "xVCamPropObj.h"
#include "xVCamObj.h"
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkTIFFWriter.h>
#include <pylon/PylonIncludes.h>
#include <GenApi.h>
#include <pylon/InstantCamera.h>
#include <QCoreApplication>
#include <QMutex>

QMutex mutex;

CPixelFormatAndAoiConfiguration::CPixelFormatAndAoiConfiguration(xVCamPropObj* p):QObject(),Pylon::CConfigurationEventHandler()
{
    pCamPropRef = p;
}
void CPixelFormatAndAoiConfiguration::OnGrabStarted( CInstantCamera& camera)
{
}
void CPixelFormatAndAoiConfiguration::OnCameraDeviceRemoved( CInstantCamera& camera)
{
    emit CameraDeviceRemoved();
}
void CPixelFormatAndAoiConfiguration::OnGrabStopped( CInstantCamera& camera)
{
    emit GrabbingStopped();
}

void CPixelFormatAndAoiConfiguration::OnOpened( Pylon::CInstantCamera& camera)
{
    // Allow all the names in the namespace GenApi to be used without qualification.
    using namespace GenApi;
    // Get the camera control object.
    INodeMap &control = camera.GetNodeMap();
    // Get the parameters for setting the image area of interest (Image AOI).
    const CIntegerPtr width = control.GetNode("Width");
    const CIntegerPtr height = control.GetNode("Height");
    const CBooleanPtr centerX = control.GetNode("CenterX");
    const CBooleanPtr centerY = control.GetNode("CenterY");
    const CFloatPtr gain = control.GetNode("Gain");
    const CFloatPtr exposureTime = control.GetNode("ExposureTime");
    const CBooleanPtr setFrameRate = control.GetNode("AcquisitionFrameRateEnable");
    const CFloatPtr aquFrameRate = control.GetNode("AcquisitionFrameRateAbs");
    CEnumerationPtr(control.GetNode("ExposureAuto"))->FromString("Off");
    CEnumerationPtr gainAuto(control.GetNode("GainAuto"));
    const CIntegerPtr binningX = control.GetNode("BinningHorizontal");
    const CIntegerPtr binningY = control.GetNode("BinningVertical");

    // Maximize the Image AOI.
    try {
        if (IsWritable(centerX)) centerX->SetValue(true);
        if (IsWritable(centerY)) centerY->SetValue(true);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }
    try {
        if (IsWritable(exposureTime)) exposureTime->SetValue((*pCamPropRef->paramMap())["exposure time [ms]"]._value.value<xLimitedInt>()._value*1000.0f);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }

    try {
        // check if in continous mode
        if (IsWritable(setFrameRate)) setFrameRate->SetValue(true);
        if (IsWritable(aquFrameRate)) aquFrameRate->SetValue((*pCamPropRef->paramMap())["frame rate [fps]"]._value.value<xLimitedInt>()._value);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }

    try {
        width->SetValue((*pCamPropRef->paramMap())["matrix width"]._value.value<xLimitedInt>()._value);
        height->SetValue((*pCamPropRef->paramMap())["matrix height"]._value.value<xLimitedInt>()._value);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }

    try {
        gain->SetValue((*pCamPropRef->paramMap())["gain [db]"]._value.value<xLimitedInt>()._value);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }
    try {
        binningX->SetValue((*pCamPropRef->paramMap())["horizontal binning"]._value.value<xLimitedInt>()._value);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }
    try {
        binningY->SetValue((*pCamPropRef->paramMap())["vertical binning"]._value.value<xLimitedInt>()._value);
    }
    catch (const Pylon::GenericException& e) {
        emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
    }
    // vertical binning
    // image mode
    CEnumerationPtr(control.GetNode("PixelFormat"))->FromString("Mono8");
}


xVCamPropObj::xVCamPropObj(const QString& txt):xVGenVisPropObj(txt){
    _type = xVOT_PYLON_CAM_PROP;
    _description = "Used to acquire images from a connected Pylon Camera device";
    _paramMp["running"]._id = 1;
    _paramMp["running"]._value = false;
    _paramMp["grab strategy"]._id = 2;
    _paramMp["grab strategy"]._value = "continous acquistion";
    _paramMp["number of images"]._id = 3;
    _paramMp["number of images"]._value = QVariant::fromValue(xLimitedInt(1,100,1));
    _paramMp["number of images"]._subGrp = "defined number of images";
    _paramMp["matrix width"]._id = 4;
    _paramMp["matrix width"]._value = QVariant::fromValue(xLimitedInt(640,320,1920));
    _paramMp["matrix height"]._id = 5;
    _paramMp["matrix height"]._value = QVariant::fromValue(xLimitedInt(480,200,1200));
    _paramMp["exposure time [ms]"]._id = 6;
    _paramMp["exposure time [ms]"]._value = QVariant::fromValue(xLimitedInt(30,5,1024));
    _paramMp["frame rate [fps]"]._id = 7;
    _paramMp["frame rate [fps]"]._value = QVariant::fromValue(xLimitedInt(50,1,1024));
    _paramMp["frame rate [fps]"]._subGrp = "continous acquistion";
    _paramMp["gain [db]"]._id = 8;
    _paramMp["gain [db]"]._value = QVariant::fromValue(xLimitedInt(3,0,1024));
    _paramMp["horizontal binning"]._id = 9;
    _paramMp["horizontal binning"]._value = QVariant::fromValue(xLimitedInt(2,1,10));
    _paramMp["vertical binning"]._id = 10;
    _paramMp["vertical binning"]._value = QVariant::fromValue(xLimitedInt(2,1,10));

    _inputRequirements << (QStringList() << "camera");

    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value = QVariant::fromValue((vtkImageDataPtr)0);
};

xVCamPropObj::xVCamPropObj(QDataStream& d):xVGenVisPropObj(d){
    _type = xVOT_PYLON_CAM_PROP;
    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value = QVariant::fromValue((vtkImageDataPtr)0);
};

void xVCamPropObj::retrieveValuesAndLimitsFromCamera()
{
    // find attached camera object
    CInstantCamera *pCamera = nullptr;
    xVCamObj *pCamObj = nullptr;
    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                xVCamObj *pCObj = dynamic_cast<xVCamObj*>(*it);
                if (pCObj && pCObj->outputParamMap()->contains("camera"))
                {
                    pCamObj=pCObj;
                    pCamera=(CInstantCamera*)((*pCamObj->outputParamMap())["camera"]._value.toULongLong());
                }
            }

    if (pCamera)
    {
        // read all current parameter
        using namespace GenApi;
        // Get the camera control object.
        INodeMap &control = pCamera->GetNodeMap();
        // Get the parameters for setting the image area of interest (Image AOI).
        const CIntegerPtr width = control.GetNode("Width");
        const CIntegerPtr height = control.GetNode("Height");
        const CBooleanPtr centerX = control.GetNode("CenterX");
        const CBooleanPtr centerY = control.GetNode("CenterY");
        const CFloatPtr gain = control.GetNode("Gain");
        const CFloatPtr exposureTime = control.GetNode("ExposureTime");
        const CBooleanPtr setFrameRate = control.GetNode("AcquisitionFrameRateEnable");
        const CFloatPtr aquFrameRate = control.GetNode("AcquisitionFrameRateAbs");
        CEnumerationPtr(control.GetNode("ExposureAuto"))->FromString("Off");
        CEnumerationPtr gainAuto(control.GetNode("GainAuto"));
        const CIntegerPtr binningX = control.GetNode("BinningHorizontal");
        const CIntegerPtr binningY = control.GetNode("BinningVertical");

        _paramMp["running"]._value = pCamThread && pCamThread->isRunning();

        try {
            _paramMp["matrix width"]._enabled = IsWritable(width);
            _paramMp["matrix width"]._value = QVariant::fromValue(xLimitedInt(width->GetValue(),width->GetMin(),width->GetMax()));
        }  catch (...) { }
        try {
            _paramMp["matrix height"]._enabled = IsWritable(height);
            _paramMp["matrix height"]._value = QVariant::fromValue(xLimitedInt(height->GetValue(),height->GetMin(),height->GetMax()));
        }  catch (...) { }
        try {
            _paramMp["horizontal binning"]._enabled = IsWritable(binningX);
            _paramMp["horizontal binning"]._value = QVariant::fromValue(xLimitedInt(binningX->GetValue(),binningX->GetMin(),binningX->GetMax()));
        }  catch (...) { }
        try {
            _paramMp["vertical binning"]._enabled = IsWritable(binningY);
            _paramMp["vertical binning"]._value = QVariant::fromValue(xLimitedInt(binningY->GetValue(),binningY->GetMin(),binningY->GetMax()));
        }  catch (...) { }
        try {
            _paramMp["exposure time [ms]"]._enabled = IsWritable(exposureTime);
            _paramMp["exposure time [ms]"]._value = QVariant::fromValue(xLimitedInt(exposureTime->GetValue()/1000,exposureTime->GetMin()/1000,exposureTime->GetMax()/1000));
        }  catch (...) { }
        try {
            _paramMp["frame rate [fps]"]._enabled = IsWritable(gain);
            _paramMp["frame rate [fps]"]._value = QVariant::fromValue(xLimitedInt(gain->GetValue(),gain->GetMin(),gain->GetMax()));
        }  catch (...) { }
        try {
            _paramMp["gain [db]"]._enabled = IsWritable(aquFrameRate);
            _paramMp["gain [db]"]._value = QVariant::fromValue(xLimitedInt(aquFrameRate->GetValue(),aquFrameRate->GetMin(),aquFrameRate->GetMax()));
        }  catch (...) { }
    }
}

void xVCamPropObj::CameraDeviceRemoved()
{
    emit KSignal(ST_ERROR_MSG,new QString("Camera removed."));
    setStatus(OS_ERROR);
    // find attached camera object
    xVCamObj *pCamObj = nullptr;
    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                xVCamObj *pCObj = dynamic_cast<xVCamObj*>(*it);
                if (pCObj && pCObj->outputParamMap()->contains("camera"))
                    pCamObj=pCObj;
            }

    if (pCamObj) pCamObj->setStatus(OS_ERROR);
}

vtkImageDataPtr vtkImageFromPylon(const CPylonImage& iimg,vtkImageData *vtkImage=nullptr)
{
    if (vtkImage==nullptr) vtkImage = vtkImageData::New();
    vtkInformation *pInfo = vtkInformation::New();
    // set data type
    long bytes = 1;
    switch (iimg.GetPixelType())
    {
    case PixelType_Mono8:
        vtkImage->SetNumberOfScalarComponents(1,pInfo);
        vtkImage->SetDimensions(iimg.GetWidth(),iimg.GetHeight(),1);
        vtkImage->SetScalarType(VTK_UNSIGNED_CHAR,pInfo);
        vtkImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
        bytes=1;
        break;
    case PixelType_Mono16:
        vtkImage->SetNumberOfScalarComponents(1,pInfo);
        vtkImage->SetDimensions(iimg.GetWidth(),iimg.GetHeight(),1);
        vtkImage->SetScalarType(VTK_UNSIGNED_SHORT,pInfo);
        vtkImage->AllocateScalars(VTK_UNSIGNED_SHORT,1);
        bytes=2;
        break;
    case PixelType_RGB8planar:
        vtkImage->SetNumberOfScalarComponents(3,pInfo);
        vtkImage->SetDimensions(iimg.GetWidth(),iimg.GetHeight(),1);
        vtkImage->SetScalarType(VTK_UNSIGNED_CHAR,pInfo);
        vtkImage->AllocateScalars(VTK_UNSIGNED_CHAR,3);
        bytes=3;
        break;
    case PixelType_RGB16planar:
        vtkImage->SetNumberOfScalarComponents(3,pInfo);
        vtkImage->SetDimensions(iimg.GetWidth(),iimg.GetHeight(),1);
        vtkImage->SetScalarType(VTK_UNSIGNED_SHORT,pInfo);
        vtkImage->AllocateScalars(VTK_UNSIGNED_SHORT,3);
        bytes=6;
        break;
    default:
        // unhandled
        break;
    }
    pInfo->Delete();

    mutex.lock();
    memcpy(vtkImage->GetScalarPointer(0,0,0),iimg.GetBuffer(),iimg.GetWidth()*iimg.GetHeight()*bytes);
    mutex.unlock();
    return vtkImage;
}

xVCamThread::xVCamThread(Pylon::CInstantCamera* pCam,xVCamPropObj* pCPO):QThread()
{
    pCamera=pCam;
    pCamPropObj=pCPO;
}
void xVCamThread::run()
{
    _abb = false;
    while (!_abb)
    {
        CGrabResultPtr ptrGrabResult;
        CPylonImage iimg;
        try {

            pCamera->RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
                iimg.AttachGrabResultBuffer(ptrGrabResult);
                (*(pCamPropObj->outputParamMap()))["image"]._value = QVariant::fromValue<vtkImageDataPtr>(vtkImageFromPylon(iimg,(*(pCamPropObj->outputParamMap()))["image"]._value.value<vtkImageDataPtr>()));
                emit update();
                //qApp->postEvent
            }
            else
            {
                //dlg->emit MSG(QString("Error: %1 %2").arg(ptrGrabResult->GetErrorCode()).arg((const char*)ptrGrabResult->GetErrorDescription()));
            }
        }
        catch (...) {

        }
    }
}
void xVCamThread::stop()
{
    _abb = true;
}

void xVCamPropObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        // find attached camera object
        CInstantCamera *pCamera = nullptr;
        xVCamObj *pCamObj = nullptr;
        for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVCamObj *pCObj = dynamic_cast<xVCamObj*>(*it);
                    if (pCObj && pCObj->outputParamMap()->contains("camera"))
                    {
                        pCamObj=pCObj;
                        pCamera=(CInstantCamera*)((*pCamObj->outputParamMap())["camera"]._value.toULongLong());
                    }
                }

        if (pCamObj && pCamera)
        {
            pCamConfig = new CPixelFormatAndAoiConfiguration(this);
            connect(pCamConfig,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL(KSignal(const SIG_TYPE&,void*)));
            connect(pCamConfig,SIGNAL(CameraDeviceRemoved()),this,SLOT(CameraDeviceRemoved()));
            connect(pCamConfig,SIGNAL(GrabbingStopped()),this,SIGNAL(GrabbingStopped()));
            // create vtk objects
            pCamera->RegisterConfiguration( pCamConfig, RegistrationMode_Append, Cleanup_Delete);
            pCamera->Open();
            bool _threaded = false;
            if (_paramMp["grab strategy"]._value.toString()=="single image")
            {
                pCamera->OutputQueueSize=1;
                pCamera->StartGrabbing(GrabStrategy_LatestImages);

            }
            if (_paramMp["grab strategy"]._value.toString()=="defined number of images")
            {
                pCamera->OutputQueueSize=1;
                pCamera->StartGrabbing(GrabStrategy_LatestImages);
            }
            if (_paramMp["grab strategy"]._value.toString()=="continous acquistion")
            {
                _threaded = true;
                pCamera->StartGrabbing(GrabStrategy_LatestImages);

                CGrabResultPtr ptrGrabResult;
                CPylonImage iimg;
                try {

                    pCamera->RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

                    // Image grabbed successfully?
                    if (ptrGrabResult->GrabSucceeded())
                    {
                        iimg.AttachGrabResultBuffer(ptrGrabResult);
                        vtkImageDataPtr pVtkImage=vtkImageFromPylon(iimg);
                        pVtkImage->Modified();
                        _outputParamMp["image"]._id=1;
                        _outputParamMp["image"]._value = QVariant::fromValue<vtkImageDataPtr>(pVtkImage);
                        emit KSignal(ST_MSG,new QString("vtkImageData generated"));
                    }
                    else
                    {
                        //dlg->emit MSG(QString("Error: %1 %2").arg(ptrGrabResult->GetErrorCode()).arg((const char*)ptrGrabResult->GetErrorDescription()));
                    }
                }
                catch (...) {

                }

                if (pCamThread==nullptr)
                {
                    pCamThread = new xVCamThread(pCamera,this);
                    connect(pCamThread,SIGNAL(update()),this,SLOT(updateImage()));
                }
                pCamThread->start();
                setStatus(OS_RUNNING);
            }
            // start acuisition
            // in case of continous acqusition we need a receiver thread

            if (!pCamThread || !pCamThread->isRunning()) setStatus(OS_VALID);
            retrieveValuesAndLimitsFromCamera();
            emit KSignal(ST_PARAMETER_LIMITS_CHANGED,this);
        }
        else
        {
            emit KSignal(ST_ERROR_MSG,new QString("no camera found"));
            setStatus(OS_ERROR);
        }
    }
}
void xVCamPropObj::updateImage()
{
    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                (*it)->paramModified();
            }
}
void xVCamPropObj::reset()
{
    // stop acquisiton
    if (pCamThread)
    {
        while (pCamThread->isRunning())
        {
            pCamThread->stop();
            QThread::msleep(25);
        }
        delete pCamThread;
        pCamThread=nullptr;
    }
    pCamConfig=nullptr;

    retrieveValuesAndLimitsFromCamera();
    emit KSignal(ST_PARAMETER_LIMITS_CHANGED,this);
    setStatus(OS_UPDATE_NEEDED);
}

void xVCamPropObj::paramModified(const QString& txt)
{
    /*
    if (_paramMp["running"]._value.toBool()) run();
    else {
        if (pCamThread)
        {
            while (pCamThread->isRunning())
            {
                pCamThread->stop();
                QThread::msleep(25);
            }
            delete pCamThread;
            pCamThread=nullptr;
        }
        pCamConfig=nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
    */
    CInstantCamera *pCamera = nullptr;
    xVCamObj *pCamObj = nullptr;
    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                xVCamObj *pCObj = dynamic_cast<xVCamObj*>(*it);
                if (pCObj && pCObj->outputParamMap()->contains("camera"))
                {
                    pCamObj=pCObj;
                    pCamera=(CInstantCamera*)((*pCamObj->outputParamMap())["camera"]._value.toULongLong());
                }
            }

    if (pCamera)
    {
        if (_paramMp["grab strategy"]._value.toString()=="continous acquistion" && pCamThread && pCamThread->isRunning() && !_paramMp["running"]._value.toBool())
        {
            pCamThread->stop();
            while (pCamThread->isRunning()) QThread::msleep(100);
            setStatus(OS_VALID);
            retrieveValuesAndLimitsFromCamera();
            emit KSignal(ST_PARAMETER_LIMITS_CHANGED,this);
        }

        using namespace GenApi;
        INodeMap &control = pCamera->GetNodeMap();
        // Get the parameters for setting the image area of interest (Image AOI).
        const CFloatPtr gain = control.GetNode("Gain");
        const CFloatPtr exposureTime = control.GetNode("ExposureTime");
        const CBooleanPtr setFrameRate = control.GetNode("AcquisitionFrameRateEnable");
        const CFloatPtr aquFrameRate = control.GetNode("AcquisitionFrameRateAbs");
        CEnumerationPtr(control.GetNode("ExposureAuto"))->FromString("Off");
        CEnumerationPtr gainAuto(control.GetNode("GainAuto"));
        try {
            if (IsWritable(exposureTime)) exposureTime->SetValue(_paramMp["exposure time [ms]"]._value.value<xLimitedInt>()._value*1000.0f);
        }
        catch (const Pylon::GenericException& e) {
            emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
        }

        try {
            if (IsWritable(setFrameRate)) setFrameRate->SetValue(true);
            if (IsWritable(aquFrameRate)) aquFrameRate->SetValue(_paramMp["frame rate [fps]"]._value.value<xLimitedInt>()._value);
        }
        catch (const Pylon::GenericException& e) {
            emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
        }

        try {
            gain->SetValue(_paramMp["gain [db]"]._value.value<xLimitedInt>()._value);
        }
        catch (const Pylon::GenericException& e) {
            emit KSignal(ST_ERROR_MSG,new QString(QString("Could not apply configuration. const GenericException caught in OnOpened method msg=")+QString(e.what())));
        }

        if (_paramMp["grab strategy"]._value.toString()=="continous acquistion" && pCamThread && !pCamThread->isRunning() && _paramMp["running"]._value.toBool())
        {
            pCamThread->start();
            setStatus(OS_RUNNING);
            retrieveValuesAndLimitsFromCamera();
            emit KSignal(ST_PARAMETER_LIMITS_CHANGED,this);
        }
    }
}
