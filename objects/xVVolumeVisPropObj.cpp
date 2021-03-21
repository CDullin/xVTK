#include "xVVolumeVisPropObj.h"

#include "xVPolyObj.h"
#include "xVVolObj.h"
#include "vtkLODProp3D.h"

#include <vtkImageReader.h>


xVVolumeVisPropObj::xVVolumeVisPropObj(const QString& txt):xVGenVisPropObj(txt)
{
    _type = xVOT_VOLUME_VIS_PROP;

    _paramMp["volume visualization style"]._id=1;
    _paramMp["volume visualization style"]._value="composite";

    vtkPiecewiseFunction* alphaChannelFunc = vtkPiecewiseFunction::New();
    _paramMp["opacity function"]._id=2;
    _paramMp["opacity function"]._value=QVariant::fromValue(alphaChannelFunc);

    vtkColorTransferFunction* colorFunc = vtkColorTransferFunction::New();
    _paramMp["color function"]._id=3;
    _paramMp["color function"]._value=QVariant::fromValue(colorFunc);

    _paramMp["ambient light power"]._id=4;
    _paramMp["ambient light power"]._value=QVariant(1.0);
    _paramMp["diffuse light power"]._id=5;
    _paramMp["diffuse light power"]._value=QVariant(1.0);
    _paramMp["specular light power"]._id=6;
    _paramMp["specular light power"]._value=QVariant(1.0);    
}

xVVolumeVisPropObj::xVVolumeVisPropObj(QDataStream &d):xVGenVisPropObj(d)
{
    _type = xVOT_VOLUME_VIS_PROP;
}

void xVVolumeVisPropObj::reset()
{
    if (_status!=OS_UPDATE_NEEDED)
    {
        // free all resources
        if (volume) volume->Delete();
        volume=nullptr;
        if (volumeMapper) volumeMapper->Delete();
        volumeMapper=nullptr;
        if (volumeProperty) volumeProperty->Delete();
        volumeProperty=nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
}

void xVVolumeVisPropObj::run()
{
    xVGenVisPropObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;
    setStatus(OS_RUNNING);
    /*
    vtkImageReader *pDataImporter = nullptr;
    for (QList <xVObj_Basics*>::iterator it=_connectedInputs.begin();it!=_connectedInputs.end();++it)
    {
        xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
        if (pIObj && pIObj->outputParamMap()->contains("volume"))
            pDataImporter=(*pIObj->outputParamMap())["volume"]._value.value<vtkImageReaderPtr>();
    }
    */
    vtkImageData *pDataImporter = nullptr;
    xVVolObj *pRefVolObj=nullptr;
    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("volume data"))
                {
                    pRefVolObj=pIObj;
                    pDataImporter=(*pIObj->outputParamMap())["volume data"]._value.value<vtkImageDataPtr>();
                }
            }

    if (pDataImporter)
    {

        volumeProperty = vtkVolumeProperty::New();
        volumeProperty->SetColor(_paramMp["color function"]._value.value<vtkColorTransferFunctionPtr>());
        volumeProperty->SetScalarOpacity(_paramMp["opacity function"]._value.value<vtkPiecewiseFunctionPtr>());

        volumeMapper = vtkFixedPointVolumeRayCastMapper::New();
        volumeMapper->SetInputData(pDataImporter);
        volumeMapper->SetBlendModeToComposite();

        volume = vtkVolume::New();
        volume->SetMapper(volumeMapper);
        volume->SetProperty(volumeProperty);

        double mi = pRefVolObj->histo()->info()._minVal;
        double ma = pRefVolObj->histo()->info()._maxVal;
        double br = ma-mi;

        vtkColorTransferFunctionPtr colorFunc = _paramMp["color function"]._value.value<vtkColorTransferFunctionPtr>();
        vtkPiecewiseFunctionPtr alphaChannelFunc = _paramMp["opacity function"]._value.value<vtkPiecewiseFunctionPtr>();

        if (alphaChannelFunc->GetSize()==0) // we need to make one
        {
            alphaChannelFunc->AddPoint(pRefVolObj->histo()->info()._dataTypeMin, 0.0);
            alphaChannelFunc->AddPoint(mi+br/3.0, 0);
            alphaChannelFunc->AddPoint(mi+2*br/3.0, 1);
            alphaChannelFunc->AddPoint(pRefVolObj->histo()->info()._dataTypeMax, 1);
        }

        if (colorFunc->GetSize()==0)
        {
            colorFunc->AddRGBPoint(pRefVolObj->histo()->info()._dataTypeMin, 0.0, 0.0, 0.0);
            colorFunc->AddRGBPoint(mi+br/3.0,0.0,0.0,0.0);
            colorFunc->AddRGBPoint(mi+2*br/3.0, 1.0, 1.0, 1.0);
            colorFunc->AddRGBPoint(pRefVolObj->histo()->info()._dataTypeMax, 1.0, 1.0, 1.0);
        }
        _outputParamMp["volume"]._value = QVariant::fromValue(volume);
        _outputParamMp["volume"]._id=0;
        if (pRefVolObj)
        {
            _outputParamMp["dimension"]._value=(*pRefVolObj->paramMap())["dimension"]._value;
            _outputParamMp["dimension"]._id=1;
            _outputParamMp["resolution"]._id=2;
            _outputParamMp["resolution"]._value=(*pRefVolObj->paramMap())["resolution"]._value;
        }
        setStatus(OS_VALID);

        paramModified("");
    }
}

void xVVolumeVisPropObj::paramModified(const QString& txt)
{
    xVGenVisPropObj::paramModified(txt);
    if (!volume) return;


    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->SetAmbient(_paramMp["ambient light power"]._value.toDouble());
    volumeProperty->SetDiffuse(_paramMp["diffuse light power"]._value.toDouble());
    volumeProperty->SetSpecular(_paramMp["specular light power"]._value.toDouble());

    switch (_optionLsts["volume visualization style"].indexOf(_paramMp["volume visualization style"]._value.toString()))
    {
    case 0 : //additiv
        volumeMapper->SetBlendModeToAdditive();
        break;
    case 1 : //average
        volumeMapper->SetBlendModeToAverageIntensity();
        break;
    case 2 : //composite
        volumeMapper->SetBlendModeToComposite();
        break;
    case 3 : //maximum
        volumeMapper->SetBlendModeToMaximumIntensity();
        break;
    case 4 : //minimum
        volumeMapper->SetBlendModeToMinimumIntensity();
        break;
    }

    pUpdateTimer->start();
}

void xVVolumeVisPropObj::save(QDataStream &d, bool _explicit)
{
    xVGenVisPropObj::save(d,_explicit);
}

void xVVolumeVisPropObj::updateTimerTimeOut()
{
   emit KSignal(ST_MSG,new QString("timeout"));
   volumeMapper->Update();
   emit parameterModified();
}
