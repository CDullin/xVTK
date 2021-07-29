#include "xVCamObj.h"
#include <pylon/PylonIncludes.h>
#include <GenApi.h>

xVCamObj::xVCamObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_PYLON_CAMERA;
    _description = "Connects to a Pylon Instant Camera device";
    _outputParamMp["connected"]._id = 0;
    _outputParamMp["connected"]._value = false;
    _outputParamMp["camera"]._id = 1;
    _outputParamMp["camera"]._value = (quint64)0;
}

xVCamObj::xVCamObj(QDataStream &d):xVGenImpObj(d)
{
    _type = xVOT_PYLON_CAMERA;
    _outputParamMp["connected"]._id = 0;
    _outputParamMp["connected"]._value = false;
    _outputParamMp["camera"]._id = 1;
    _outputParamMp["camera"]._value = (quint64)0;
}

void xVCamObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);
        // try to connect to a camera device
        PylonInitialize();
        try {
            pCamera=new CInstantCamera (CTlFactory::GetInstance().CreateFirstDevice());

            _outputParamMp["connected"]._id = 0;
            _outputParamMp["connected"]._value = pCamera->IsPylonDeviceAttached();
            _outputParamMp["camera"]._id = 1;
            _outputParamMp["camera"]._value = (quint64)pCamera;

        }  catch (...) {

            _outputParamMp["connected"]._id = 0;
            _outputParamMp["connected"]._value = false;
            _outputParamMp["camera"]._id = 1;
            _outputParamMp["camera"]._value = 0;
        }

        // add more model, serial number etc.

        if (_outputParamMp["connected"]._value.toBool()) setStatus(OS_VALID);
        else
        {
            emit KSignal(ST_ERROR_MSG,new QString("Couldn't connect to any pylon camera"));
            setStatus(OS_ERROR);
        }
    }
}

void xVCamObj::reset()
{
    // disconnect camera
    setStatus(OS_UPDATE_NEEDED);
    if (pCamera) pCamera->Close();
    delete pCamera;
    pCamera=nullptr;
}
