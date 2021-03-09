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
        dlg.addInputParam("global namespace",&::_globalNameSpace,0);
        for (QList <xConnector*>::iterator it2=pRefObj->connectorLst()->begin();it2!=pRefObj->connectorLst()->end();++it2)
        {
            // find all connected and enabled parameter connections
            if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
                for (QList<xVObj_Basics*>::iterator it3=(*it2)->connectedObjects()->begin();it3!=(*it2)->connectedObjects()->end();++it3)
                    dlg.addInputParam((*(*it3)->paramMap())["name"]._value.toString(),(*it3)->paramMap(),(*it3));
        }


        dlg.setOutputParam(&_paramMap,pRefObj);
        dlg.setToControlMode();
        dlg.exec();

        setText(QString("define parameter [%1]").arg(_paramMap.count()));
        emit modified();
    }

    if (pVarDefObj)
    {
        xVUserTableDefinitionDlg dlg;
        dlg.setOutputParam(&_paramMap,pVarDefObj);
        dlg.setToDefinitionMode();
        connect(&dlg,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SIGNAL(KSignal(const SIG_TYPE&,void *)));
        connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),&dlg,SLOT(KSlot(const SIG_TYPE&,void *)));
        dlg.exec();


        setText(QString("variables [%1]").arg(_paramMap.count()));
        emit modified();
    }
}

xParamMap xVUserTableDefinitionDlgItem::paramMap(){return _paramMap;}
void xVUserTableDefinitionDlgItem::setMap(xParamMap m,xVUserTableImportDlgObj*p){_paramMap=m;pRefObj=p;}
void xVUserTableDefinitionDlgItem::setMap(xParamMap m,xVVarDefinitionObj*p){_paramMap=m;pVarDefObj=p;}

