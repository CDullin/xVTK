#include "xVUserTableDefinitionDlgItem.h"
#include "xvusertabledefinitiondlg.h"
#include "xVObjectTypes.h"

xVUserTableDefinitionDlgItem::xVUserTableDefinitionDlgItem(QWidget* parent):QToolButton(parent)
{
    setText("define parameter");
    connect(this,SIGNAL(clicked()),this,SLOT(clicked_SLOT()));
}

void xVUserTableDefinitionDlgItem::clicked_SLOT()
{
    if (pRefObj)
    {
        xVUserTableDefinitionDlg dlg;
        dlg.addInputParam("global",&::_globalNameSpace,0);
        for (QList <xConnector*>::iterator it2=pRefObj->connectorLst()->begin();it2!=pRefObj->connectorLst()->end();++it2)
        {
            // find all connected and enabled parameter connections
            if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
                for (QList<xVObj_Basics*>::iterator it3=(*it2)->connectedObjects()->begin();it3!=(*it2)->connectedObjects()->end();++it3)
                    dlg.addInputParam((*(*it3)->paramMap())["name"]._value.toString(),(*it3)->paramMap(),nullptr,(*it3));
        }


        dlg.setOutputParam((*pRefObj->paramMap())["parameter table"]._value.value<xParamMap>(),pRefObj);
        dlg.setToControlMode();
        dlg.setOutputConnectionsEnabled(false);
        dlg.exec();
        (*pRefObj->paramMap())["parameter table"]._value=QVariant::fromValue<xParamMap>(dlg.resultingMap());

        setText(QString("define parameter [%1]").arg((*pRefObj->paramMap())["parameter table"]._value.value<xParamMap>().count()));
        emit modified();
    }

    if (pReportObj)
    {
        xVUserTableDefinitionDlg dlg;
        dlg.addInputParam("global",&::_globalNameSpace,0);
        for (QList <xConnector*>::iterator it2=pReportObj->connectorLst()->begin();it2!=pReportObj->connectorLst()->end();++it2)
        {
            // find all connected and enabled parameter connections
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList<xVObj_Basics*>::iterator it3=(*it2)->connectedObjects()->begin();it3!=(*it2)->connectedObjects()->end();++it3)
                    dlg.addInputParam((*(*it3)->paramMap())["name"]._value.toString(),(*it3)->paramMap(),(*it3)->outputParamMap(),(*it3));
        }

        dlg.setOutputParam((*pReportObj->paramMap())["parameter"]._value.value<xParamMap>(),pReportObj);
        dlg.setToControlMode();
        dlg.setOutputConnectionsEnabled(true);
        dlg.exec();
        (*pReportObj->paramMap())["parameter"]._value=QVariant::fromValue<xParamMap>(dlg.resultingMap());

        setText(QString("define parameter [%1]").arg((*pReportObj->paramMap())["parameter"]._value.value<xParamMap>().count()));
        emit modified();
    }

    if (pVarDefObj)
    {
        xVUserTableDefinitionDlg dlg;
        dlg.setOutputParam((*pVarDefObj->paramMap())["parameter table"]._value.value<xParamMap>(),pVarDefObj);
        dlg.setToDefinitionMode();
        dlg.setOutputConnectionsEnabled(false);
        connect(&dlg,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SIGNAL(KSignal(const SIG_TYPE&,void *)));
        connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),&dlg,SLOT(KSlot(const SIG_TYPE&,void *)));
        dlg.exec();
        (*pVarDefObj->paramMap())["parameter table"]._value=QVariant::fromValue<xParamMap>(dlg.resultingMap());
        setText(QString("variables [%1]").arg((*pVarDefObj->paramMap())["parameter table"]._value.value<xParamMap>().count()));
        emit modified();
    }
}

void xVUserTableDefinitionDlgItem::setRef(xVUserTableImportDlgObj*p){pRefObj=p;}
void xVUserTableDefinitionDlgItem::setRef(xVVarDefinitionObj*p){pVarDefObj=p;}
void xVUserTableDefinitionDlgItem::setRef(xVReportObj *p){pReportObj=p;}

