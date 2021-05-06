#include "xVPolygonizationFilterObj.h"
#include "xVVolObj.h"

xVPolygonizationFilterObj::xVPolygonizationFilterObj(const QString& txt):xVGenFilterObj(txt)
{
    _type = xVOT_POLYGONIZATION;
    _paramMp["threshold"]._id=1;
    _paramMp["threshold"]._value=QVariant::fromValue(xLimitedDouble(12000,-100000,100000,10));
    pProgObserver = new xVProgressObserver(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
    _outputParamMp["mesh"]._value=QVariant::fromValue((vtkPolyDataMapperPtr) 0);
}

xVPolygonizationFilterObj::xVPolygonizationFilterObj(QDataStream &d):xVGenFilterObj(d)
{
    _type = xVOT_POLYGONIZATION;
    pProgObserver = new xVProgressObserver(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
    _outputParamMp["mesh"]._value=QVariant::fromValue((vtkPolyDataMapperPtr) 0);
}

void xVPolygonizationFilterObj::reset()
{
    _outputParamMp["mesh"]._value=QVariant::fromValue((vtkPolyDataMapperPtr) 0);
    if (pMapper) pMapper->RemoveAllInputs();pMapper->Delete();pMapper=nullptr;
    if (pMarchingCubes) pMarchingCubes->RemoveAllInputs();pMarchingCubes->Delete();pMarchingCubes=nullptr;
    setStatus(OS_UPDATE_NEEDED);
}

void xVPolygonizationFilterObj::run()
{
    xVGenFilterObj::run();
    if (_status == OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        vtkImageData *pData  = nullptr;
        xVVolObj *pVolObjRef = nullptr;
        for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                    if (pIObj && pIObj->outputParamMap()->contains("volume data"))
                    {
                        pVolObjRef=pIObj;
                        pData=(*pIObj->outputParamMap())["volume data"]._value.value<vtkImageDataPtr>();
                    }

                    xVGenFilterObj *pFObj = dynamic_cast<xVGenFilterObj*>(*it);
                    if (pFObj && pFObj->outputParamMap()->contains("image"))
                    {
                        pVolObjRef = (xVVolObj*)((*pFObj->outputParamMap())["volume object"]._value.toULongLong());
                        vtkImageDataPtr pImgData = (*pFObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
                        int dim[4];
                        if (pImgData)
                        {
                             pImgData->GetDimensions(dim);
                             if (dim[2]>1)
                                 pData=pImgData;
                        }
                    }
                }

        if (pData && pVolObjRef)
        {
            pMarchingCubes = vtkMarchingCubes::New();
            pMarchingCubes->SetInputData(pData);
            pMarchingCubes->SetValue(0, _paramMp["threshold"]._value.value<xLimitedDouble>()._value);           // contour id, iso value
            emit KSignal(ST_SET_PROCESS_RANGE,
                         new QPoint(
                             pMarchingCubes->GetProgressMinValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value,
                             pMarchingCubes->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value));
            emit KSignal(ST_SET_PROCESS_TXT,new QString("polygonization ... "));
            pMarchingCubes->SetProgressObserver(pProgObserver);
            pMarchingCubes->Update();
            pMapper = vtkPolyDataMapper::New();
            pMapper->SetInputData(pMarchingCubes->GetOutput());
            _outputParamMp["mesh"]._value=QVariant::fromValue((vtkPolyDataMapperPtr) pMapper);
            setStatus(OS_VALID);
        }
        else
        {
            emit KSignal(ST_ERROR_MSG,new QString("no input data found"));
            setStatus(OS_ERROR);
        }
    }
}
