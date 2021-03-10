#include "xVEquationDlgItem.h"
#include "xvequationdesigndlg.h"

xVEquationDlgItem::xVEquationDlgItem(xVObj_Basics* pRO,QWidget* parent):QToolButton(parent)
{
    pRefObj=pRO;
    setText("define equation");
    connect(this,SIGNAL(clicked()),this,SLOT(clicked_SLOT()));
}
QString xVEquationDlgItem::equation()
{
    return _equation;
}

void xVEquationDlgItem::clicked_SLOT()
{
    if (pRefObj)
    {
        xVEquationDesignDlg dlg;
        dlg.addInputParam("global namespace",&::_globalNameSpace,0);
        for (QList <xConnector*>::iterator it2=pRefObj->connectorLst()->begin();it2!=pRefObj->connectorLst()->end();++it2)
        {
            // find all connected and enabled parameter connections
            if ((*it2)->isEnabled())
                for (QList<xVObj_Basics*>::iterator it3=(*it2)->connectedObjects()->begin();it3!=(*it2)->connectedObjects()->end();++it3)
                    dlg.addInputParam((*(*it3)->paramMap())["name"]._value.toString(),(*it3)->paramMap(),(*it3));
        }
        dlg.exec();

        setText(dlg.equation());
        emit modified();
    }
}
