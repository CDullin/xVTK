#include "xVLSIFilterObj.h"
#include "xVObjectTypes.h"
#include "vtkImageMedian3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageConvolve.h"

// https://vtk.org/doc/nightly/html/classvtkThreadedImageAlgorithm.html

// we do the rest with vtkImageConvolve.h

xVLSIFilterObj::xVLSIFilterObj(const QString& txt):xVGenFilterObj(txt)
{
    // split parameter per filter
    _type=xVOT_LSI_FILTER;
    _paramMp["filter type"]._value="median";
    _paramMp["filter type"]._id=1;
    _optionLsts["filter type"] << "median" << "gaussian" << "sobel";
    _paramMp["median kernel size"]._id=2;
    _paramMp["median kernel size"]._value=QVariant::fromValue(xLimitedInt(3,1,20));
    _paramMp["median kernel size"]._subGrp="median";
    _paramMp["radius factor"]._id=3;
    _paramMp["radius factor"]._subGrp="gaussian";
    _paramMp["radius factor"]._value=QVariant::fromValue(xLimitedDouble(2.5,0.1,100,10));

    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value = QVariant::fromValue((vtkImageDataPtr)0);
    pProgObserver = new xVProgressObserver(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
}
xVLSIFilterObj::xVLSIFilterObj(QDataStream &d):xVGenFilterObj(d)
{
    _type=xVOT_LSI_FILTER;
    _optionLsts["filter type"] << "median" << "gaussian" << "sobel";
    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value = QVariant::fromValue((vtkImageDataPtr)0);
    _outputParamMp["volume object"]._id=2;
    _outputParamMp["volume object"]._value = qulonglong(0);
    pProgObserver = new xVProgressObserver(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
}
void xVLSIFilterObj::run()
{
    xVGenFilterObj::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        vtkImageData *pData = nullptr;
        for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVVolObj *pVObj = dynamic_cast<xVVolObj*>(*it);
                    if (pVObj && pVObj->outputParamMap()->contains("volume data"))
                    {
                        pData=(*pVObj->outputParamMap())["volume data"]._value.value<vtkImageDataPtr>();
                        _outputParamMp["volume object"]._value=(qulonglong)pVObj;
                    }

                    xVImageObj *pIObj = dynamic_cast<xVImageObj*>(*it);
                    if (pIObj && pIObj->outputParamMap()->contains("image"))
                        pData=(*pIObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
                }

        if (pData)
        {
            if (pAlgorithm)
                pAlgorithm->Delete();
            pAlgorithm = nullptr;
            switch (_optionLsts["filter type"].indexOf(_paramMp["filter type"]._value.toString()))
            {
            case 0 : // median
                {
                    vtkImageMedian3D *pMedianFilter = vtkImageMedian3D::New();
                    int kernelSize = _paramMp["kernel size"]._value.value<xLimitedInt>()._value;
                    pMedianFilter->SetKernelSize(kernelSize,kernelSize,kernelSize);
                    pAlgorithm = pMedianFilter;
                }
                break;
            case 1 : // gaussian
                {
                    // continue here
                    vtkImageGaussianSmooth *pGaussianSmoothFilter = vtkImageGaussianSmooth::New();
                    pGaussianSmoothFilter->SetRadiusFactor(_paramMp["radius factor"]._value.value<xLimitedDouble>()._value);
                    pAlgorithm = pGaussianSmoothFilter;
                }
                break;
            case 2 : // sobel
                {
                    double dimz[]={1,2,1,2,4,2,1,2,1,0,0,0,0,0,0,0,0,0,-1,-2,-1,-2,-4,-2,-1,-2,-1};
                    double dimx[]={1,2,1,0,0,0,-1,-2,-1,2,4,2,0,0,0,-2,-4,-2,1,2,1,0,0,0,-1,-2,-1};
                    double dimy[]={1,0,-1,2,0,-2,1,0,-1,2,0,-2,4,0,-4,2,0,-2,1,0,-1,2,0,-2,1,0,-1};
                    vtkImageConvolve *pSobelXFilter = vtkImageConvolve::New();
                    pSobelXFilter->SetKernel3x3x3(dimx);
                    pSobelXFilter->SetInputData(pData);
                    pSobelXFilter->Update();
                    vtkImageConvolve *pSobelYFilter = vtkImageConvolve::New();
                    pSobelYFilter->SetKernel3x3x3(dimy);
                    pSobelYFilter->SetInputData(pData);
                    pSobelYFilter->Update();
                    vtkImageConvolve *pSobelZFilter = vtkImageConvolve::New();
                    pSobelZFilter->SetKernel3x3x3(dimz);
                    pSobelZFilter->SetInputData(pData);
                    pSobelZFilter->Update();
                    // calculate (x^2+y^2+z^2)^0.5

                }
                break;
            default:
                emit KSignal(ST_ERROR_MSG,new QString("unknown filter type specified"));
                break;
            }

            if (pAlgorithm)
            {
                pAlgorithm->SetInputData(pData);
                emit KSignal(ST_SET_PROCESS_RANGE,
                             new QPoint(
                                 pAlgorithm->GetProgressMinValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value,
                                 pAlgorithm->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value));
                emit KSignal(ST_SET_PROCESS_TXT,new QString("filtering ... "));
                pAlgorithm->SetProgressObserver(pProgObserver);
                pAlgorithm->Update();
                _outputParamMp["image"]._value = QVariant::fromValue((vtkImageDataPtr)pAlgorithm->GetOutput());
            }

            setStatus(OS_VALID);
        }
        else
        {
            emit KSignal(ST_ERROR_MSG,new QString("No input image data found"));
            setStatus(OS_ERROR);
        }

    }
}
