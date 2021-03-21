#include "xV3DVisObj.h"
#include "xVGenVisPropObj.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkCubeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkLODProp3D.h"
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
    _paramMp["use shadows"]._id = 9;
    _paramMp["use shadows"]._value = true;
    _paramMp["hidden line removal"]._id = 10;
    _paramMp["hidden line removal"]._value = true;
    _paramMp["orientation marker"]._id=11;
    _paramMp["orientation marker"]._value = true;
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
        pRenderer->SetUseDepthPeeling(false);
        pRenderer->SetUseDepthPeelingForVolumes(false);
        pRenderer->SetUseHiddenLineRemoval(true);
        pRenderWdgt->GetRenderWindow()->SetLineSmoothing(1);
    }

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
                    vtkVolume *pVtkVol=(*pIObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                    pRenderer->AddVolume(pVtkVol);
                }
            }
        }

        pOrientationMaker = vtkOrientationMarkerWidget::New();
        pOrientationMaker->SetInteractor(pRenderWdgt->GetInteractor());
        pOrientationMaker->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
        pAxes = vtkAxesActor::New();
        pOrientationMaker->SetOrientationMarker(pAxes);
        pOrientationMaker->SetViewport( 0.0, 0.0, 0.4, 0.4 );
        pOrientationMaker->SetEnabled( 1 );
        pOrientationMaker->InteractiveOn();
    }

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

    pRenderWdgt->update();
}

void xV3DVisObj::save(QDataStream &d, bool _explicit)
{
    xVGenVisObj::save(d,_explicit);
    // continue here
}
