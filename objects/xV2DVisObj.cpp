#include "xV2DVisObj.h"
#include "xVTypes.h"
#include "xVGenVisPropObj.h"
#include <QVector3D>
#include "vtkAbstractVolumeMapper.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkImageMapper3D.h"
#include "vtkVolumeProperty.h"
#include "vtkImageMapper.h"
#include "vtkInformation.h"
#include "vtkMapper.h"
#include "vtkActor2D.h"
#include "vtkImageChangeInformation.h"
#include "vtkCamera.h"
#include "xVObjectTypes.h"
#include "xVGenImpObj.h"

#include <stdlib.h>
using namespace std;

xV2DVisObj::xV2DVisObj(const QString& txt):xVGenVisObj(txt)
{
    _type = xVOT_2D_VIEW;
    _paramMp["2dview orientation"]._id = 1;
    _paramMp["2dview orientation"]._value = "xy-plane";
    _paramMp["frame position"]._id = 2;
    _paramMp["frame position"]._value = 0;
    _paramMp["center"]._id = 3;
    _paramMp["center"]._value = QVariant::fromValue(QVector3D(256,256,256));
    _paramMp["center"]._subGrp="free-plane";
    _paramMp["direction"]._id = 4;
    _paramMp["direction"]._value = QVariant::fromValue(QVector3D(0,0,1));
    _paramMp["direction"]._subGrp="free-plane";
    _paramMp["scaling"]._id = 5;
    _paramMp["scaling"]._value = QVariant::fromValue(xLimitedDouble(1,0.01,5,100));
    _paramMp["background color1"]._id = 10;
    _paramMp["background color1"]._value = QVariant::fromValue(QColor(100,100,100));
    _paramMp["background color2"]._id = 11;
    _paramMp["background color2"]._value = QVariant::fromValue(QColor(200,100,100));
    _paramMp["background gradient"]._id = 12;
    _paramMp["background gradient"]._value = true;
    _maxInput = 1000;
}

xV2DVisObj::xV2DVisObj(QDataStream &d):xVGenVisObj(d)
{
    _type = xVOT_2D_VIEW;
}

void xV2DVisObj::reset()
{
    if (_status!=OS_UPDATE_NEEDED)
    {
        // remove visualization widget
        emit KSignal(ST_REMOVE_VISUALIZATION_WDGT,pRenderWdgt);
        // free all resources
        pRenderer->Delete();
        if (p2DActor) p2DActor->Delete();
        if (pIActor) pIActor->Delete();
        if (pColorMapper) pColorMapper->Delete();
        if (pSlicer) pSlicer->Delete();
        if (pSliceOrientation) pSliceOrientation->Delete();
        pRenderWdgt=nullptr;
        pRenderer=nullptr;
        pIActor=nullptr;
        p2DActor=nullptr;
        pColorMapper=nullptr;
        pSlicer=nullptr;
        pSliceOrientation=nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
}

void xV2DVisObj::run_volume_data()
{
    vtkVolume *pVtkVol=nullptr;
    x3D_SAMPLE_POS dimension(1,1,1);

    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVGenVisPropObj *pIObj = dynamic_cast<xVGenVisPropObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("volume"))
                    pVtkVol=(*pIObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                if (pIObj && pIObj->outputParamMap()->contains("dimension"))
                    dimension=(*pIObj->outputParamMap())["dimension"]._value.value<x3D_SAMPLE_POS>();
            }
        }
    }

    if (!pVtkVol) return;
    vtkVolumeProperty* volumeProperty = pVtkVol->GetProperty();
    if (!volumeProperty) return;
    vtkColorTransferFunctionPtr pColorFunc=volumeProperty->GetRGBTransferFunction();
    if (!pColorFunc) return;

    if (pVtkVol)
    {
        // get center
        double center[3];
        center[0] = _paramMp["center"]._value.value<QVector3D>().x();
        center[1] = _paramMp["center"]._value.value<QVector3D>().y();
        center[2] = _paramMp["center"]._value.value<QVector3D>().z();

        static double axialElements[16] = {
                 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1 };
        static double coronalElements[16] = {
                 1, 0, 0, 0,
                 0, 0, 1, 0,
                 0,-1, 0, 0,
                 0, 0, 0, 1 };

        static double sagittalElements[16] = {
                0, 0, -1, 0,
                1, 0, 0, 0,
                0, -1, 0, 0,
                0, 0, 0, 1};

        pSliceOrientation = vtkMatrix4x4::New();
        switch (::_optionLsts["2dview orientation"].indexOf(_paramMp["2dview orientation"]._value.toString()))
        {
        case 0 : // xy
            pSliceOrientation->DeepCopy(axialElements);
            // set limits for slider
            _paramMp["frame position"]._value=QVariant::fromValue(xLimitedInt(max((quint32)0,min(dimension.z,(quint32)_paramMp["frame position"]._value.value<xLimitedInt>()._value)),0,dimension.z));
            break;
        case 1 : // xz
            pSliceOrientation->DeepCopy(coronalElements);
            _paramMp["frame position"]._value=QVariant::fromValue(xLimitedInt(max((quint32)0,min(dimension.y,(quint32)_paramMp["frame position"]._value.value<xLimitedInt>()._value)),0,dimension.y));
            break;
        case 2 : // yz
            pSliceOrientation->DeepCopy(sagittalElements);
            _paramMp["frame position"]._value=QVariant::fromValue(xLimitedInt(max((quint32)0,min(dimension.x,(quint32)_paramMp["frame position"]._value.value<xLimitedInt>()._value)),0,dimension.x));
            break;
        case 3 : // free ()
            break;
        }

        pSliceOrientation->SetElement(0,3,_paramMp["frame position"]._value.value<xLimitedInt>()._value);
        pSliceOrientation->SetElement(1,3,_paramMp["frame position"]._value.value<xLimitedInt>()._value);
        pSliceOrientation->SetElement(2,3,_paramMp["frame position"]._value.value<xLimitedInt>()._value);

        pSlicer = vtkImageReslice::New();
        pSlicer->SetInputData(pVtkVol->GetMapper()->GetDataSetInput());
        pSlicer->SetOutputDimensionality(2);
        pSlicer->SetResliceAxes(pSliceOrientation);
        pSlicer->SetInterpolationModeToLinear();

        // Map the image through the lookup table
        pColorMapper = vtkImageMapToColors::New();
        pColorMapper->SetLookupTable(pColorFunc);
        pColorMapper->SetInputConnection(pSlicer->GetOutputPort());

        // Display the image
        pIActor = vtkImageActor::New();
        pIActor->GetMapper()->SetInputConnection(pColorMapper->GetOutputPort());

        pRenderer->AddActor(pIActor);

        emit KSignal(ST_ADD_VISUALIZATION_WDGT,pRenderWdgt);
        setStatus(OS_VALID);
    }
}

void xV2DVisObj::run_image_data()
{
    vtkImageData *pVTKImage = nullptr;
    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVGenVisPropObj *pIObj = dynamic_cast<xVGenVisPropObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("image"))
                    pVTKImage=(*pIObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
                xVGenFilterObj *pFObj = dynamic_cast<xVGenFilterObj*>(*it);
                if (pFObj && pFObj->outputParamMap()->contains("image"))
                {
                    vtkImageDataPtr pImage = (*pFObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
                    if (pImage)
                    {
                        int dim[4];pImage->GetDimensions(dim);
                        if (dim[2]<2) pVTKImage=pImage;
                    }
                }
                // find all import obj
                xVGenImpObj *pInObj = dynamic_cast<xVGenImpObj*>(*it);
                if (pInObj && pInObj->outputParamMap()->contains("image"))
                    pVTKImage=(*pInObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
            }
        }
    }

    if (!pVTKImage) return;


    vtkImageMapper *pMapper = vtkImageMapper::New();
    pMapper->SetInputData(pVTKImage);
    pMapper->SetColorLevel(127.5);
    pMapper->SetColorWindow(255);

    p2DActor = vtkActor2D::New();
    p2DActor->SetMapper(pMapper);

    pRenderer->AddActor(p2DActor);

    int dim[4];
    int size[2];
    pVTKImage->GetDimensions((int*)&dim);
    size[0]=(pRenderWdgt->GetRenderWindow()->GetSize())[0];
    size[1]=(pRenderWdgt->GetRenderWindow()->GetSize())[1];
    p2DActor->SetPosition((float)(size[0]-dim[0])/2.0,(float)(size[1]-dim[1])/2.0);

    emit KSignal(ST_ADD_VISUALIZATION_WDGT,pRenderWdgt);
    setStatus(OS_VALID);
}

void xV2DVisObj::run()
{
    xVGenVisObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;

    setStatus(OS_RUNNING);
    if (pRenderWdgt==nullptr)
    {

        pRenderWdgt = new QVTKWidget();
        pRenderWdgt->resize( 512, 512);
        pRenderer = vtkRenderer::New();
        pRenderWdgt->GetRenderWindow()->AddRenderer(pRenderer);
        pRenderer->SetUseDepthPeeling(false);
        pRenderer->SetUseDepthPeelingForVolumes(false);
        pRenderer->SetUseHiddenLineRemoval(true);
    }

    run_volume_data();
    run_image_data();

    //pRenderer->GetActiveCamera()->SetParallelScale(_paramMp["scaling"]._value.value<xLimitedDouble>()._value);
}

void xV2DVisObj::update_volume_data()
{
    vtkVolume *pVtkVol=nullptr;
    // connect to data
    x3D_SAMPLE_POS dimension(1,1,1);
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVGenVisPropObj *pIObj = dynamic_cast<xVGenVisPropObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("volume"))
                {
                    pVtkVol=(*pIObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                    dimension=(*pIObj->outputParamMap())["dimension"]._value.value<x3D_SAMPLE_POS>();
                }
            }
        }
    }

    if (!pVtkVol) return;
    vtkVolumeProperty* volumeProperty = pVtkVol->GetProperty();
    if (!volumeProperty) return;
    vtkColorTransferFunctionPtr pColorFunc=volumeProperty->GetRGBTransferFunction();
    if (!pColorFunc) return;
    // get center

    double center[3];
    center[0] = _paramMp["center"]._value.value<QVector3D>().x();
    center[1] = _paramMp["center"]._value.value<QVector3D>().y();
    center[2] = _paramMp["center"]._value.value<QVector3D>().z();

    static double axialElements[16] = {
             1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1 };
    static double coronalElements[16] = {
             1, 0, 0, 0,
             0, 0, -1, 0,
             0,1, 0, 0,
             0, 0, 0, 1 };

    static double sagittalElements[16] = {
            0, 0, 1, 0,
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 1};

    switch (::_optionLsts["2dview orientation"].indexOf(_paramMp["2dview orientation"]._value.toString()))
    {
    case 0 : // xy
        pSliceOrientation->DeepCopy(axialElements);
        pSliceOrientation->SetElement(2,3,_paramMp["frame position"]._value.value<xLimitedInt>()._value);
        break;
    case 1 : // xz
        pSliceOrientation->DeepCopy(coronalElements);
        pSliceOrientation->SetElement(1,3,_paramMp["frame position"]._value.value<xLimitedInt>()._value);
        break;
    case 2 : // yz
        pSliceOrientation->DeepCopy(sagittalElements);
        pSliceOrientation->SetElement(0,3,_paramMp["frame position"]._value.value<xLimitedInt>()._value);
        break;
    case 3 : // free ()
        break;
    }

    pSlicer->Update();
    pIActor->Update();
}

void xV2DVisObj::update_image_data()
{
    vtkImageData *pVTKImage = nullptr;
    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVGenVisPropObj *pIObj = dynamic_cast<xVGenVisPropObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("image"))
                    pVTKImage=(*pIObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();                
                xVGenFilterObj *pGFObj = dynamic_cast<xVGenFilterObj*>(*it);
                if (pGFObj && pGFObj->outputParamMap()->contains("image"))
                {
                    vtkImageDataPtr pImg = (*pIObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
                    if (pImg)
                    {
                        int dim[4];pImg->GetDimensions(dim);
                        if (dim[2]<2) pVTKImage=pImg;
                    }
                }
                // find all import obj
                xVGenImpObj *pInObj = dynamic_cast<xVGenImpObj*>(*it);
                if (pInObj && pInObj->outputParamMap()->contains("image"))
                    pVTKImage=(*pInObj->outputParamMap())["image"]._value.value<vtkImageDataPtr>();
            }
        }
    }

    if (!pVTKImage || !pRenderer) return;
    int dim[4];
    int size[2];
    pVTKImage->GetDimensions((int*)&dim);
    size[0]=(pRenderWdgt->GetRenderWindow()->GetSize())[0];
    size[1]=(pRenderWdgt->GetRenderWindow()->GetSize())[1];
    p2DActor->SetPosition((float)(size[0]-dim[0])/2.0,(float)(size[1]-dim[1])/2.0);
}

void xV2DVisObj::paramModified(const QString& txt)
{
    xVGenVisObj::paramModified(txt);    
    if (!pRenderWdgt) return;

    update_volume_data();
    update_image_data();

    QColor c1=_paramMp["background color1"]._value.value<QColor>();
    pRenderer->SetBackground(c1.redF(),c1.greenF(),c1.blueF());
    QColor c2=_paramMp["background color2"]._value.value<QColor>();
    pRenderer->SetBackground2(c2.redF(),c2.greenF(),c2.blueF());
    pRenderer->SetGradientBackground(_paramMp["background gradient"]._value.toBool());

    //pRenderer->GetActiveCamera()->SetParallelScale(_paramMp["scaling"]._value.value<xLimitedDouble>()._value);
    pRenderWdgt->update();
}

void xV2DVisObj::save(QDataStream &d, bool _explicit)
{
    xVGenVisObj::save(d,_explicit);
    // continue here
}
