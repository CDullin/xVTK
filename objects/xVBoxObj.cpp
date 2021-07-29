#include "xVBoxObj.h"
#include "QVTKInteractor.h"
#include "xVVolumeVisPropObj.h"
#include "xV3DVisObj.h"
#include "vtkWidgetRepresentation.h"
#include "vtkActor.h"
#include "xVVolObj.h"
#include "vtkVolume.h"

xVBoxObj::xVBoxObj(const QString& txt):xVGenVisPropObj(txt)
{
    _type = xVOT_CLIPPING_BOX;
    _description = "displays an interactive box that\ncan be used to clip a volume data set";

    _paramMp["visible"]._id=1;
    _paramMp["visible"]._value=true;
    _paramMp["place factor"]._id=10;
    _paramMp["place factor"]._value=QVariant::fromValue(xLimitedDouble(1,0.1,2.0,10));
    _paramMp["active"]._id=11;
    _paramMp["active"]._value=true;
    _paramMp["interaction mode"]._id=12;
    if (!_optionLsts["interaction mode"].contains("transformation")) _optionLsts["interaction mode"] << "transformation";
    if (!_optionLsts["interaction mode"].contains("clipping")) _optionLsts["interaction mode"] << "clipping";
    _paramMp["interaction mode"]._value="clipping";
    _paramMp["inverted"]._id=13;
    _paramMp["inverted"]._subGrp="clipping";
    _paramMp["inverted"]._value=false;

    _paramMp["translation enabled"]._id=20;
    _paramMp["translation enabled"]._value=false;
    _paramMp["rotation enabled"]._id=21;
    _paramMp["rotation enabled"]._value=false;
    _paramMp["scaling enabled"]._id=22;
    _paramMp["scaling enabled"]._value=true;

    _outputParamMp["actor"]._value = QVariant::fromValue((vtkActorPtr)0);
    _outputParamMp["actor"]._id=0;
}
xVBoxObj::xVBoxObj(QDataStream &d):xVGenVisPropObj(d)
{
    _type = xVOT_CLIPPING_BOX;

    _outputParamMp["actor"]._value = QVariant::fromValue((vtkActorPtr)0);
    _outputParamMp["actor"]._id=0;
}

void xVBoxObj::run()
{
    xVGenVisPropObj::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        paramModified("");

        setStatus(OS_VALID);
    }
}

void xVBoxObj::save(QDataStream& d, bool _explicit)
{
    xVGenVisPropObj::save(d,_explicit);
}

void xVBoxObj::reset()
{}

void xVBoxObj::paramModified(const QString& txt)
{
    xVGenVisPropObj::paramModified(txt);

    QVTKInteractor* pInteractor=nullptr;
    vtkActor *pActor=nullptr;
    vtkVolume *pVolume=nullptr;

    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
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

    if (!pVolume) {
        emit KSignal(ST_WARN_MSG,new QString("This object needs to be accompanied with an volume property object"));
        return;
    }
    if (!pBoxWdgt) pBoxWdgt=vtkBoxWidget::New();
    if (!pBoxCallBack) pBoxCallBack=xVBoxCallback::New();

    pBoxWdgt->SetProp3D(pVolume);
    pBoxWdgt->SetPlaceFactor(_paramMp["place factor"]._value.value<xLimitedDouble>()._value); // Default is 0.5
    pBoxWdgt->SetInteractor(pInteractor);
    pBoxWdgt->PlaceWidget();
    pBoxWdgt->SetEnabled(_paramMp["active"]._value.toBool());
    pBoxWdgt->SetInsideOut(_paramMp["inverted"]._value.toBool());
    pBoxWdgt->SetRotationEnabled(_paramMp["rotation enabled"]._value.toBool());
    pBoxWdgt->SetTranslationEnabled(_paramMp["translation enabled"]._value.toBool());
    pBoxWdgt->SetScalingEnabled(_paramMp["scaling enabled"]._value.toBool());

    // Set up a callback for the interactor to call so we can manipulate the actor
    pBoxWdgt->AddObserver(vtkCommand::InteractionEvent, pBoxCallBack);

    _outputParamMp["actor"]._value = QVariant::fromValue((vtkActorPtr)pActor);
    _outputParamMp["actor"]._id=0;

    _paramMp["visible"]._value.toBool() ? pBoxWdgt->On() : pBoxWdgt->Off();
}
