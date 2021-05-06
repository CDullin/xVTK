#include "xV3DVisObj.h"
#include "xVGenVisPropObj.h"
#include "xVTextPropertyDlgItem.h"
#include "xVAxisPropertyDlgItem.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkCubeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkLODProp3D.h"
#include "vtkLegendScaleActor.h"
#include "vtkCamera.h"
#include "vtkLightCollection.h"
#include "vtkLight.h"
#include "vtkImagePlaneWidget.h"
#include "xVTextPropertyDlgItem.h"
#include "vtkBoxWidget.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkDataSet.h"
#include <QDockWidget>
#include <QProcess>
#include <QTextStream>
#include <QWindow>
#include <QCoreApplication>

xV3DVisObj::xV3DVisObj(const QString& txt):xVGenVisObj(txt)
{
    _type = xVOT_3D_VIEW;
    _maxInput = 1000;

    _paramMp["background color1"]._id = 1;
    _paramMp["background color1"]._value = QVariant::fromValue(QColor(100,100,100));
    _paramMp["background color2"]._id = 2;
    _paramMp["background color2"]._value = QVariant::fromValue(QColor(200,100,100));
    _paramMp["background gradient"]._id = 3;
    _paramMp["background gradient"]._value = true;
    _paramMp["point smoothing"]._id = 4;
    _paramMp["point smoothing"]._value = true;
    _paramMp["line smoothing"]._id = 5;
    _paramMp["line smoothing"]._value = true;
    _paramMp["polygon smoothing"]._id = 6;
    _paramMp["polygon smoothing"]._value = true;
    _paramMp["light follows camera"]._id = 7;
    _paramMp["light follows camera"]._value = true;
    _paramMp["double sided lighting"]._id = 8;
    _paramMp["double sided lighting"]._value = true;
    _paramMp["light intensity"]._id = 9;
    _paramMp["light intensity"]._value = QVariant::fromValue(xLimitedDouble(0.5,0,1.0,100));
    _paramMp["use shadows"]._id = 20;
    _paramMp["use shadows"]._value = true;
    _paramMp["hidden line removal"]._id = 21;
    _paramMp["hidden line removal"]._value = true;
    _paramMp["orientation marker"]._id=22;
    _paramMp["orientation marker"]._value = true;
    _paramMp["scale bar"]._id=30;
    _paramMp["scale bar"]._value = true;
    _paramMp["scale bar label mode"]._id=31;
    _paramMp["scale bar label mode"]._value="distance";
    _optionLsts["scale bar label mode"] << "distance" << "XY coordinates";
    _paramMp["right axis visible"]._id=32;
    _paramMp["right axis visible"]._value=true;
    _paramMp["left axis visible"]._id=33;
    _paramMp["left axis visible"]._value=true;
    _paramMp["top axis visible"]._id=34;
    _paramMp["top axis visible"]._value=true;
    _paramMp["bottom axis visible"]._id=35;
    _paramMp["bottom axis visible"]._value=true;
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
}

xV3DVisObj::xV3DVisObj(QDataStream &d):xVGenVisObj(d)
{
    _type = xVOT_3D_VIEW;
    _maxInput = 1000;
}

void xV3DVisObj::run()
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
        pRenderer->SetUseDepthPeeling(true);
        pRenderer->SetUseDepthPeelingForVolumes(true);
        pRenderer->SetUseHiddenLineRemoval(true);
        pRenderer->SetAutomaticLightCreation(true);
        pRenderWdgt->GetRenderWindow()->SetLineSmoothing(1);
    }

    vtkVolume *pVtkVol=nullptr;

    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVGenVisPropObj *pIObj = dynamic_cast<xVGenVisPropObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("actor"))
                    pRenderer->AddActor((*pIObj->outputParamMap())["actor"]._value.value<vtkActorPtr>());
                if (pIObj && pIObj->outputParamMap()->contains("volume"))
                {
                    pVtkVol=(*pIObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                    pRenderer->AddVolume(pVtkVol);
                }
                /*
                if ((*it)->outputParamMap()->contains("bounding box"))
                {
                    vtkActor *pBBoxActor = vtkActor::New();
                    pBBoxActor->SetMapper((*(*it)->outputParamMap())["bounding box"]._value.value<vtkPolyDataMapperPtr>());
                    pRenderer->AddActor(pBBoxActor);
                }
                */
            }
        }
    }

    pRenderer->GetActiveCamera()->ParallelProjectionOn();
    pRenderer->ResetCameraClippingRange();
    pRenderer->ResetCamera();

    pOrientationMaker = vtkOrientationMarkerWidget::New();
    pOrientationMaker->SetInteractor(pRenderWdgt->GetInteractor());
    pOrientationMaker->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    pAxes = vtkAxesActor::New();
    pOrientationMaker->SetOrientationMarker(pAxes);
    pOrientationMaker->SetViewport( 0.0, 0.0, 0.4, 0.4 );
    pOrientationMaker->SetEnabled( 1 );
    pOrientationMaker->InteractiveOn();

    pScaleBar = vtkLegendScaleActor::New();
    _paramMp["scale bar title font"]._value.value<xVTextPropPtr>()->generateParamFromVtkTextProp(pScaleBar->GetLegendTitleProperty());
    _paramMp["scale bar label font"]._value.value<xVTextPropPtr>()->generateParamFromVtkTextProp(pScaleBar->GetLegendLabelProperty());
    _paramMp["right axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetRightAxis());
    _paramMp["left axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetLeftAxis());
    _paramMp["top axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetTopAxis());
    _paramMp["bottom axis properties"]._value.value<xVAxisPropPtr>()->generateParamFromVtkAxisProp(pScaleBar->GetBottomAxis());
    pRenderer->AddActor(pScaleBar);

    /* works
    vtkBoxWidget *pBBoxWdgt = vtkBoxWidget::New();
    pBBoxWdgt->SetInteractor(pRenderWdgt->GetInteractor());
    pBBoxWdgt->SetProp3D(pVtkVol);
    pBBoxWdgt->PlaceWidget();
    pBBoxWdgt->On();
    */

    vtkImagePlaneWidget *pPlaneWdgt = vtkImagePlaneWidget::New();
    pPlaneWdgt->SetInteractor(pRenderWdgt->GetInteractor());
    pPlaneWdgt->TextureVisibilityOn();
    pPlaneWdgt->SetInputData(pVtkVol->GetMapper()->GetDataSetInput());
    pPlaneWdgt->SetPlaneOrientationToYAxes();
    pPlaneWdgt->SetSlicePosition(10);
    pPlaneWdgt->UpdatePlacement();
    pPlaneWdgt->On();

    // adjusts the initial parameter
    paramModified("");

    emit KSignal(ST_ADD_VISUALIZATION_WDGT,pRenderWdgt);
    setStatus(OS_VALID);
}

void xV3DVisObj::reset()
{
    if (_status!=OS_UPDATE_NEEDED)
    {
        // remove visualization widget
        emit KSignal(ST_REMOVE_VISUALIZATION_WDGT,pRenderWdgt);
        // free all resources
        pRenderer->Delete();
        pRenderWdgt=nullptr;
        pRenderer=nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
}

void xV3DVisObj::paramModified(const QString &txt)
{
    xVGenVisObj::paramModified(txt);

    if(!pRenderWdgt) return;

    QColor c1=_paramMp["background color1"]._value.value<QColor>();
    pRenderer->SetBackground(c1.redF(),c1.greenF(),c1.blueF());
    QColor c2=_paramMp["background color2"]._value.value<QColor>();
    pRenderer->SetBackground2(c2.redF(),c2.greenF(),c2.blueF());
    _paramMp["light follows camera"]._value.toBool() ? pRenderer->LightFollowCameraOn() : pRenderer->LightFollowCameraOff();
    pRenderer->SetTwoSidedLighting(_paramMp["double sided lighting"]._value.toBool());
    _paramMp["use shadows"]._value.toBool() ? pRenderer->UseShadowsOn() : pRenderer->UseShadowsOff();
    _paramMp["hidden line removal"]._value.toBool() ? pRenderer->UseHiddenLineRemovalOn() : pRenderer->UseHiddenLineRemovalOff();
    pRenderer->SetGradientBackground(_paramMp["background gradient"]._value.toBool());

    _paramMp["point smoothing"]._value.toBool() ? pRenderWdgt->GetRenderWindow()->PointSmoothingOn() : pRenderWdgt->GetRenderWindow()->PointSmoothingOff();
    _paramMp["line smoothing"]._value.toBool() ? pRenderWdgt->GetRenderWindow()->LineSmoothingOn() : pRenderWdgt->GetRenderWindow()->LineSmoothingOff();
    _paramMp["polygon smoothing"]._value.toBool() ? pRenderWdgt->GetRenderWindow()->PolygonSmoothingOn() : pRenderWdgt->GetRenderWindow()->PolygonSmoothingOff();
    pRenderWdgt->setProperty("title",_paramMp["name"]._value);


    QDockWidget* pDWdgt = dynamic_cast <QDockWidget*>(pRenderWdgt->parentWidget());
    if (pDWdgt) pDWdgt->setProperty("windowTitle",_paramMp["name"]._value);

    //widget->SetInteractor( renderWindowInteractor );
    pOrientationMaker->SetEnabled( _paramMp["orientation marker"]._value.toBool() );
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

    float intensity = _paramMp["light intensity"]._value.value<xLimitedDouble>()._value;
    vtkLightCollection* pLightLst=pRenderer->GetLights();
    vtkLight *pLight=pLightLst->GetNextItem();
    while (pLight)
    {
        pLight->SetIntensity(intensity);
        pLight=pLightLst->GetNextItem();
    }
    pRenderWdgt->update();
}

void xV3DVisObj::save(QDataStream &d, bool _explicit)
{
    xVGenVisObj::save(d,_explicit);
    // continue here
}
