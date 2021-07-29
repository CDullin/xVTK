#include "xVPlaneObj.h"
#include "xVVolObj.h"
#include "xV3DVisObj.h"
#include "vtkImagePlaneWidget.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkImageReslice.h"
#include "vtkPlane.h"
#include "QVTKInteractor.h"
#include <QVector4D>
#include <QMatrix4x4>
#include "xVVolumeVisPropObj.h"

xVPlaneObj::xVPlaneObj(const QString& txt):xVGenVisPropObj(txt)
{
    qRegisterMetaType<vtkImagePlaneWidgetPtr>("vtkImagePlaneWidgetPtr");
    _type = xVOT_CLIPPING_PLANE;
    _description="Interactive reslicing plane of a volume\ndata set that can be display in 2D and 3D";

    _paramMp["visible"]._id=1;
    _paramMp["visible"]._value=true;
    _paramMp["texture visible"]._id=2;
    _paramMp["texture visible"]._value=true;
    _paramMp["outline color"]._id = 3;
    _paramMp["outline color"]._value=QVariant::fromValue(QColor(Qt::white));
    _paramMp["clipping"]._id=4;
    _paramMp["clipping"]._value=true;
    _paramMp["inverted clipping"]._id=5;
    _paramMp["inverted clipping"]._value=false;

    _paramMp["center x [mm]"]._id=10;
    _paramMp["center x [mm]"]._value=QVariant::fromValue(xLimitedDouble(0,1,0,100));
    _paramMp["center y [mm]"]._id=11;
    _paramMp["center y [mm]"]._value=QVariant::fromValue(xLimitedDouble(0,1,0,100));
    _paramMp["center z [mm]"]._id=12;
    _paramMp["center z [mm]"]._value=QVariant::fromValue(xLimitedDouble(0,1,0,100));

    _paramMp["alpha [°]"]._id=16;
    _paramMp["alpha [°]"]._value=QVariant::fromValue(xLimitedDouble(0,-180,180,10));
    _paramMp["phi [°]"]._id=17;
    _paramMp["phi [°]"]._value=QVariant::fromValue(xLimitedDouble(0,-180,180,10));
    _paramMp["teta [°]"]._id=18;
    _paramMp["teta [°]"]._value=QVariant::fromValue(xLimitedDouble(0,-180,180,10));

    _outputParamMp["plane widget"]._id=1;
    _outputParamMp["plane widget"]._value=QVariant::fromValue((vtkImagePlaneWidgetPtr)0);

    _inputRequirements << (QStringList() << "volume data");
}

xVPlaneObj::xVPlaneObj(QDataStream& d):xVGenVisPropObj(d)
{
    qRegisterMetaType<vtkImagePlaneWidgetPtr>("vtkImagePlaneWidgetPtr");
    _type = xVOT_CLIPPING_PLANE;
    _outputParamMp["plane widget"]._id=1;
    _outputParamMp["plane widget"]._value=QVariant::fromValue((vtkImagePlaneWidgetPtr)0);
}

void xVPlaneObj::run()
{
    xVGenVisPropObj::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        vtkImageData *pData = nullptr;
        vtkVolume *pVolume=nullptr;
        QVTKInteractor* pInteractor=nullptr;
        for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        {
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                    if ((*it)->outputParamMap()->contains("volume data"))
                        pData =(*pIObj->outputParamMap())["volume data"]._value.value<vtkImageDataPtr>();
                    xVVolObj* pVObj = dynamic_cast<xVVolObj*>(*it);
                    if (pVObj)
                    {
                        // we need the vis properties
                        for (QList <xConnector*>::iterator it3=pVObj->connectorLst()->begin();it3!=pVObj->connectorLst()->end();++it3)
                        {
                            if ((*it3)->type()==xCT_OUTPUT)
                            {
                                for (QList <xVObj_Basics*>::iterator it4=(*it3)->connectedObjects()->begin();it4!=(*it3)->connectedObjects()->end();++it4)
                                {
                                    xVVolumeVisPropObj* pVVisObj=dynamic_cast<xVVolumeVisPropObj*>(*it4);
                                    if (pVVisObj)
                                        pVolume =(*pVVisObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                                }
                            }
                        }
                    }
                 }
            if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xV3DVisObj *p3DVisObj = dynamic_cast<xV3DVisObj*>(*it);
                    if ((*it)->outputParamMap()->contains("interactor"))
                        pInteractor =(*p3DVisObj->outputParamMap())["interactor"]._value.value<QVTKInteractor*>();
                }
        }


        if (pData)
        {
            pPlaneWdgt = vtkImagePlaneWidget::New();
            pPlaneWdgt->SetInputData(pData);
            pPlaneWdgt->SetInteractor(pInteractor);
            QColor col=_paramMp["outline color"]._value.value<QColor>();
            pPlaneWdgt->GetPlaneProperty()->SetColor(col.redF(),col.greenF(),col.blueF());

            //pPlaneWdgt->SetProp3D(vtkActor::New());
            pPlaneWdgt->SetPlaneOrientationToYAxes();
            double bounds[6];
            pPlaneWdgt->GetInput()->GetBounds(bounds);
            pPlaneWdgt->SetSlicePosition((bounds[4]+bounds[5])/2.0);
            pPlaneWdgt->UpdatePlacement();
            pPlaneWdgt->On();

            pCallBack = xVPlaneCallback::New();
            pCallBack->SetVolume(pVolume);
            pPlaneWdgt->AddObserver(vtkCommand::InteractionEvent, pCallBack);

            // update parameter
            _paramMp["center x [mm]"]._value=QVariant::fromValue(xLimitedDouble((bounds[0]+bounds[1])/2.0,bounds[0],bounds[1],100));
            _paramMp["center y [mm]"]._value=QVariant::fromValue(xLimitedDouble((bounds[2]+bounds[3])/2.0,bounds[2],bounds[3],100));
            _paramMp["center z [mm]"]._value=QVariant::fromValue(xLimitedDouble((bounds[4]+bounds[5])/2.0,bounds[4],bounds[5],100));

            _outputParamMp["plane widget"]._value=QVariant::fromValue((vtkImagePlaneWidgetPtr)pPlaneWdgt);
        }

        paramModified("");

        setStatus(OS_VALID);
    }
}

void xVPlaneObj::paramModified(const QString& txt)
{
    xVGenVisPropObj::paramModified(txt);
    vtkImageData *pData = nullptr;
    QVTKInteractor* pInteractor=nullptr;
    vtkVolume *pVolume=nullptr;
    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                if ((*it)->outputParamMap()->contains("volume data"))
                {
                    pData =(*pIObj->outputParamMap())["volume data"]._value.value<vtkImageDataPtr>();
                }
                xVVolObj* pVObj = dynamic_cast<xVVolObj*>(*it);
                if (pVObj)
                {
                    // we need the vis properties
                    for (QList <xConnector*>::iterator it3=pVObj->connectorLst()->begin();it3!=pVObj->connectorLst()->end();++it3)
                    {
                        if ((*it3)->type()==xCT_OUTPUT)
                        {
                            for (QList <xVObj_Basics*>::iterator it4=(*it3)->connectedObjects()->begin();it4!=(*it3)->connectedObjects()->end();++it4)
                            {
                                xVVolumeVisPropObj* pVVisObj=dynamic_cast<xVVolumeVisPropObj*>(*it4);
                                if (pVVisObj)
                                    pVolume =(*pVVisObj->outputParamMap())["volume"]._value.value<vtkVolumePtr>();
                            }
                        }
                    }
                }
            }
    if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
        for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
        {
            xV3DVisObj *p3DVisObj = dynamic_cast<xV3DVisObj*>(*it);
            if ((*it)->outputParamMap()->contains("interactor"))
                pInteractor =(*p3DVisObj->outputParamMap())["interactor"]._value.value<QVTKInteractor*>();
        }
    }

    if (pPlaneWdgt && pData)
    {
        pPlaneWdgt->SetInputData(pData);
        pPlaneWdgt->SetInteractor(pInteractor);
        pPlaneWdgt->SetTextureVisibility(_paramMp["texture visible"]._value.toBool());
        _paramMp["visible"]._value.toBool() ? pPlaneWdgt->On() : pPlaneWdgt->Off();
        QColor col=_paramMp["outline color"]._value.value<QColor>();
        pPlaneWdgt->GetPlaneProperty()->SetColor(col.redF(),col.greenF(),col.blueF());

        double center[3];pPlaneWdgt->GetCenter(center);

        // move
        double o1[3],p1[3],p2[3];
        double vec[3]={
            _paramMp["center x [mm]"]._value.value<xLimitedDouble>()._value-center[0],
            _paramMp["center y [mm]"]._value.value<xLimitedDouble>()._value-center[1],
            _paramMp["center z [mm]"]._value.value<xLimitedDouble>()._value-center[2]};
        pPlaneWdgt->GetOrigin(o1);
        pPlaneWdgt->GetPoint1(p1);
        pPlaneWdgt->GetPoint2(p2);
        o1[0]+=vec[0];
        o1[1]+=vec[1];
        o1[2]+=vec[2];
        p1[0]+=vec[0];
        p1[1]+=vec[1];
        p1[2]+=vec[2];
        p2[0]+=vec[0];
        p2[1]+=vec[1];
        p2[2]+=vec[2];
        pPlaneWdgt->SetOrigin(o1);
        pPlaneWdgt->SetPoint1(p1);
        pPlaneWdgt->SetPoint2(p2);

        // rotate
        // get center -> set center
        double c[3];
        pPlaneWdgt->GetOrigin(o1);
        pPlaneWdgt->GetPoint1(p1);
        pPlaneWdgt->GetPoint2(p2);
        pPlaneWdgt->GetCenter(c);

        QVector <QVector4D> _points;
        _points.append(QVector4D(o1[0],o1[1],o1[2],1));
        _points.append(QVector4D(p1[0],p1[1],p1[2],1));
        _points.append(QVector4D(p2[0],p2[1],p2[2],1));

        QVector4D _center(c[0],c[1],c[2],1);

        vec[0]=_paramMp["alpha [°]"]._value.value<xLimitedDouble>()._value;
        vec[1]=_paramMp["phi [°]"]._value.value<xLimitedDouble>()._value;
        vec[2]=_paramMp["teta [°]"]._value.value<xLimitedDouble>()._value;

        QMatrix4x4 R;
        R.rotate(vec[0],1,0,0);
        R.rotate(vec[1],0,1,0);
        R.rotate(vec[2],0,0,1);

        // generate translation matrix to move to the center
        for (int i=0;i<_points.count();++i)
        {
            QVector4D p = _points[i]-_center;
            p = R*p;
            _points[i]=p+_center;
        }
        pPlaneWdgt->SetOrigin(_points[0][0],_points[0][1],_points[0][2]);
        pPlaneWdgt->SetPoint1(_points[1][0],_points[1][1],_points[1][2]);
        pPlaneWdgt->SetPoint2(_points[2][0],_points[2][1],_points[2][2]);

        pPlaneWdgt->UpdatePlacement();
        pCallBack->SetVolume(pVolume);
        _paramMp["clipping"]._value.toBool() ? pCallBack->On() : pCallBack->Off();
        pCallBack->setInverted(_paramMp["inverted clipping"]._value.toBool());
        pCallBack->Execute(pPlaneWdgt,0,0);

        // trigger the connected objects
        updateConnectedObjects();

        emit parameterModified();
    }
}
