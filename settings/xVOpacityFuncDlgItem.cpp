#include "xVOpacityFuncDlgItem.h"
#include "xVVolObj.h"
#include "xVObjects.h"
#include "xVVolumeVisPropObj.h"
#include "xvhistodlg.h"

#include <QResizeEvent>

xVOpacityFuncDlgItem::xVOpacityFuncDlgItem(QWidget *parent):QLabel(parent)
{
    setGeometry(0,0,100,20);
    generatePixmap();
}
void xVOpacityFuncDlgItem::setOpcityFunc(vtkPiecewiseFunctionPtr pORef)
{
    pOpacityFunc=pORef;
    generatePixmap();
}
void xVOpacityFuncDlgItem::setRefVolumeObj(xVVolumeVisPropObj* pRObj)
{
    pVolVisPropObj=pRObj;
    generatePixmap();
}

void xVOpacityFuncDlgItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    xVVolObj* pVObj = nullptr;
    if (!pVolVisPropObj) return;
    else
    {
        for (QList <xConnector*>::iterator it2=pVolVisPropObj->connectorLst()->begin();it2!=pVolVisPropObj->connectorLst()->end();++it2)
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                    if (pIObj && pIObj->outputParamMap()->contains("volume data"))
                        pVObj=pIObj;
                }
    }

    if (!pVObj) return;
    xVHistoDlg dlg;
    dlg.setMode(xVHistoDlg::HDM_OPACITY);
    dlg.setHisto(pVObj->histo());
    dlg.setOpacityFunc((*pVolVisPropObj->paramMap())["opacity function"]._value.value<vtkPiecewiseFunctionPtr>());
    connect(&dlg,SIGNAL(modified(const QString&)),pVolVisPropObj,SLOT(paramModified(const QString&)));
    connect(&dlg,SIGNAL(KSignal(const SIG_TYPE& ,void *)),this,SIGNAL(KSignal(const SIG_TYPE& ,void *)));
    dlg.exec();
    generatePixmap();
}

void xVOpacityFuncDlgItem::resizeEvent(QResizeEvent *event)
{
    QSize size=event->size();
    setGeometry(0,0,size.width(),size.height());
    generatePixmap();
}

void xVOpacityFuncDlgItem::generatePixmap()
{
    if (!pOpacityFunc) return;
    xVVolObj* pVObj = nullptr;
    if (!pVolVisPropObj) return;
    else
    {
        for (QList <xConnector*>::iterator it2=pVolVisPropObj->connectorLst()->begin();it2!=pVolVisPropObj->connectorLst()->end();++it2)
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                    if (pIObj && pIObj->outputParamMap()->contains("volume data"))
                        pVObj=pIObj;
                }
    }

    if (!pVObj || !pVObj->histo()) return;

    setPixmap(xVHistoDlg::bar(
                  xVHistoDlg::HDM_OPACITY,
                  size().width(),size().height(),
                  pVObj->histo()->info()._dataTypeMin,
                  pVObj->histo()->info()._dataTypeMax,
                  (*pVolVisPropObj->paramMap())["opacity function"]._value.value<vtkPiecewiseFunctionPtr>()));
}
