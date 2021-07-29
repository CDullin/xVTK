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
#include "xVPlaneObj.h"
#include "vtkRendererCollection.h"
#include "xVTextPropertyDlgItem.h"
#include "xVAxisPropertyDlgItem.h"

#include <stdlib.h>
using namespace std;

xV2DVisObj::xV2DVisObj(const QString& txt):xVGenVisObj(txt)
{
    _description = "displays images as well\nas virtual cuts of 3D data sets";
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
    _paramMp["background color2"]._value = QVariant::fromValue(QColor(200,200,200));
    _paramMp["background gradient"]._id = 12;
    _paramMp["background gradient"]._value = true;
    _maxInput = 1000;
    _paramMp["scale bar"]._id=30;
    _paramMp["scale bar"]._value = true;
    _paramMp["scale bar label mode"]._id=31;
    _paramMp["scale bar label mode"]._value="distance";
    if (!_optionLsts["scale bar label mode"].contains("distance")) _optionLsts["scale bar label mode"] << "distance";
    if (!_optionLsts["scale bar label mode"].contains("XY coordinates")) _optionLsts["scale bar label mode"] << "XY coordinates";
    _paramMp["right axis visible"]._id=32;
    _paramMp["right axis visible"]._value=false;
    _paramMp["left axis visible"]._id=33;
    _paramMp["left axis visible"]._value=false;
    _paramMp["top axis visible"]._id=34;
    _paramMp["top axis visible"]._value=false;
    _paramMp["bottom axis visible"]._id=35;
    _paramMp["bottom axis visible"]._value=false;
    _paramMp["scale bar legend visible"]._id=36;
    _paramMp["scale bar legend visible"]._value=true;
    _paramMp["right axis offset"]._id=37;
    _paramMp["right axis offset"]._value=QVariant::fromValue(xLimitedInt(20,0,1000));
    _paramMp["left axis offset"]._id=38;
    _paramMp["left axis offset"]._value=QVariant::fromValue(xLimitedInt(20,0,1000));
    _paramMp["top axis offset"]._id=39;
    _paramMp["top axis offset"]._value=QVariant::fromValue(xLimitedInt(20,0,1000));
    _paramMp["bottom axis offset"]._id=40;
    _paramMp["bottom axis offset"]._value=QVariant::fromValue(xLimitedInt(20,0,1000));
    _paramMp["scale bar title font"]._id=41;
    xVTextProp *pTxtProp = new xVTextProp();
    _paramMp["scale bar title font"]._value=QVariant::fromValue(pTxtProp);
    pTxtProp = new xVTextProp();
    _paramMp["scale bar label font"]._id=42;
    _paramMp["scale bar label font"]._value=QVariant::fromValue(pTxtProp);
    _paramMp["right axis properties"]._id=50;
    _paramMp["right axis properties"]._value=QVariant::fromValue(new xVAxisProp());
    _paramMp["left axis properties"]._id=51;
    _paramMp["left axis properties"]._value=QVariant::fromValue(new xVAxisProp());
    _paramMp["top axis properties"]._id=52;
    _paramMp["top axis properties"]._value=QVariant::fromValue(new xVAxisProp());
    _paramMp["bottom axis properties"]._id=53;
    _paramMp["bottom axis properties"]._value=QVariant::fromValue(new xVAxisProp());

    _inputRequirements << (QStringList() << "image");
    _inputRequirements << (QStringList() << "volume");
    _inputRequirements << (QStringList() << "plane widget" << "volume");
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
    bool _slicerFound=false;

    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT &&
            (*it2)->isEnabled() &&
            (*it2)->baseObj()==_dashboardLst[_currentDashBoard]->pPrevStepObj)
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVGenVisPropObj *pIObj = dynamic_cast<xVGenVisPropObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("volume"))
                    pVtkVol=(*pIObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                if (pIObj && pIObj->outputParamMap()->contains("dimension"))
                    dimension=(*pIObj->outputParamMap())["dimension"]._value.value<x3D_SAMPLE_POS>();
                if ((*it)->outputParamMap()->contains("plane widget")) _slicerFound=true;
            }
        }
    }
    if (_slicerFound) return;

    if (!pVtkVol) return;
    vtkVolumeProperty* volumeProperty = pVtkVol->GetProperty();
    if (!volumeProperty) return;
    vtkColorTransferFunctionPtr pColorFunc=volumeProperty->GetRGBTransferFunction();
    if (!pColorFunc) return;

    if (pVtkVol)
    {
        emit KSignal(ST_MSG,new QString("run volume data"));

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
        if ((*it2)->type()==xCT_INPUT &&
            (*it2)->isEnabled() &&
            (*it2)->baseObj()==_dashboardLst[_currentDashBoard]->pPrevStepObj)
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

                // find plane objects
                if ((*it)->outputParamMap()->contains("plane"))
                {
                    vtkImagePlaneWidgetPtr pPlane=(*(*it)->outputParamMap())["plane"]._value.value<vtkImagePlaneWidgetPtr>();
                    if (pPlane)
                    {
                        pVTKImage = pPlane->GetResliceOutput();
                    }
                }
            }
        }
    }

    if (!pVTKImage) return;

    emit KSignal(ST_MSG,new QString("run image data"));

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

    setStatus(OS_VALID);
}

void xV2DVisObj::run()
{
    xVGenVisObj::run();

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

        pScaleBar = vtkLegendScaleActor::New();
        _paramMp["scale bar title font"]._value.value<xVTextPropPtr>()->generateParamFromVtkTextProp(pScaleBar->GetLegendTitleProperty());
        _paramMp["scale bar label font"]._value.value<xVTextPropPtr>()->generateParamFromVtkTextProp(pScaleBar->GetLegendLabelProperty());
        _paramMp["right axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetRightAxis());
        _paramMp["left axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetLeftAxis());
        _paramMp["top axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetTopAxis());
        _paramMp["bottom axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetBottomAxis());
        pRenderer->AddActor(pScaleBar);
        pRenderWdgt->setProperty("title",QVariant(_paramMp["name"]._value.toString()));
        emit KSignal(ST_ADD_VISUALIZATION_WDGT,pRenderWdgt);
    }

    run_volume_data();
    run_image_data();
    run_cut_plane();

    setStatus(OS_VALID);
}

void xV2DVisObj::update_volume_data()
{
    vtkVolume *pVtkVol=nullptr;
    bool _slicerFound=false;
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
                if ((*it)->outputParamMap()->contains("plane widget")) _slicerFound=true;
            }
        }
    }
    if (_slicerFound) return;
    if (!pVtkVol) return;
    vtkVolumeProperty* volumeProperty = pVtkVol->GetProperty();
    if (!volumeProperty) return;
    vtkColorTransferFunctionPtr pColorFunc=volumeProperty->GetRGBTransferFunction();
    if (!pColorFunc) return;
    // get center

    emit KSignal(ST_MSG,new QString("update volume data"));

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

void xV2DVisObj::run_cut_plane()
{
    vtkImagePlaneWidget *planeWdgt = nullptr;
    vtkVolume *pVolume = nullptr;
    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                if ((*it)->outputParamMap()->contains("plane widget"))
                    planeWdgt = (*(*it)->outputParamMap())["plane widget"]._value.value<vtkImagePlaneWidgetPtr>();
                if ((*it)->outputParamMap()->contains("volume"))
                    pVolume=(*(*it)->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
            }
        }
    }

    if (pVolume && planeWdgt)
    {
        emit KSignal(ST_MSG,new QString("run cut plane"));
        vtkVolumeProperty* volumeProperty = pVolume->GetProperty();
        vtkColorTransferFunction* pColorFunc=volumeProperty->GetRGBTransferFunction();
        // Map the image through the lookup table
        vtkImageMapToColors* pColorMapper = vtkImageMapToColors::New();
        pColorMapper->SetLookupTable(pColorFunc);
        pColorMapper->SetInputConnection(planeWdgt->GetReslice()->GetOutputPort());

        pIActor = vtkImageActor::New();
        pIActor->GetMapper()->SetInputConnection(pColorMapper->GetOutputPort());
        pRenderWdgt->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(pIActor);

        vtkCamera *pCamera = pRenderWdgt->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
        vtkRenderer *pRenderer = pRenderWdgt->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        pCamera->ParallelProjectionOn();
        pRenderer->ResetCameraClippingRange();
        pRenderer->ResetCamera();
    }
}

void xV2DVisObj::update_cut_plane()
{
    /*
    vtkImagePlaneWidget *planeWdgt = nullptr;
    vtkVolume *pVolume = nullptr;
    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                if ((*it)->paramMap()->contains("plane widget"))
                    planeWdgt = (*(*it)->paramMap())["plane widget"]._value.value<vtkImagePlaneWidgetPtr>();
                if ((*it)->outputParamMap()->contains("volume"))
                    pVolume=(*(*it)->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
            }
        }
    }

    if (pVolume && planeWdgt)
    {
        emit KSignal(ST_MSG,new QString("update cut plane"));
        vtkVolumeProperty* volumeProperty = pVolume->GetProperty();
        vtkColorTransferFunction* pColorFunc=volumeProperty->GetRGBTransferFunction();
        // Map the image through the lookup table
        vtkImageMapToColors* pColorMapper = vtkImageMapToColors::New();
        pColorMapper->SetLookupTable(pColorFunc);
        pColorMapper->SetInputConnection(planeWdgt->GetReslice()->GetOutputPort());

        pIActor = vtkImageActor::New();
        pIActor->GetMapper()->SetInputConnection(pColorMapper->GetOutputPort());
        pRenderWdgt->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(pIActor);

        vtkCamera *pCamera = pRenderWdgt->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
        vtkRenderer *pRenderer = pRenderWdgt->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        pCamera->ParallelProjectionOn();
        pRenderer->ResetCameraClippingRange();
        pRenderer->ResetCamera();
    }
    */
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
    update_cut_plane();

    QColor c1=_paramMp["background color1"]._value.value<QColor>();
    pRenderer->SetBackground(c1.redF(),c1.greenF(),c1.blueF());
    QColor c2=_paramMp["background color2"]._value.value<QColor>();
    pRenderer->SetBackground2(c2.redF(),c2.greenF(),c2.blueF());
    pRenderer->SetGradientBackground(_paramMp["background gradient"]._value.toBool());

    pScaleBar->SetVisibility(_paramMp["scale bar"]._value.toBool() );
    _paramMp["scale bar label mode"]._value.toString()=="distance" ? pScaleBar->SetLabelModeToDistance() : pScaleBar->SetLabelModeToXYCoordinates();
    pScaleBar->SetRightAxisVisibility(_paramMp["right axis visible"]._value.toBool());
    pScaleBar->SetLeftAxisVisibility(_paramMp["left axis visible"]._value.toBool());
    pScaleBar->SetTopAxisVisibility(_paramMp["top axis visible"]._value.toBool());
    pScaleBar->SetBottomAxisVisibility(_paramMp["bottom axis visible"]._value.toBool());
    pScaleBar->SetLegendVisibility(_paramMp["scale bar legend visible"]._value.toBool());
    pScaleBar->SetRightBorderOffset(_paramMp["right axis offset"]._value.value<xLimitedInt>()._value);
    pScaleBar->SetLeftBorderOffset(_paramMp["left axis offset"]._value.value<xLimitedInt>()._value);
    pScaleBar->SetTopBorderOffset(_paramMp["top axis offset"]._value.value<xLimitedInt>()._value);
    pScaleBar->SetBottomBorderOffset(_paramMp["bottom axis offset"]._value.value<xLimitedInt>()._value);
    pScaleBar->Modified();

    pRenderWdgt->update();
}

void xV2DVisObj::save(QDataStream &d, bool _explicit)
{
    xVGenVisObj::save(d,_explicit);
    // continue here
}
