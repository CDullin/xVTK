#include "xVLUTFuncDlgItem.h"

#include "xVVolObj.h"
#include "xVObjects.h"
#include "xVVolumeVisPropObj.h"
#include "xvhistodlg.h"

#include <QResizeEvent>

xVLUTFuncDlgItem::xVLUTFuncDlgItem(QWidget *parent):QLabel(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setGeometry(0,0,100,20);
    generatePixmap();
}
void xVLUTFuncDlgItem::setLUTFunc(vtkColorTransferFunctionPtr pORef)
{
    pLutFunc=pORef;
    generatePixmap();
}
void xVLUTFuncDlgItem::setRefVolumeObj(xVVolumeVisPropObj* pRObj)
{
    pVolVisPropObj=pRObj;
    generatePixmap();
}
void xVLUTFuncDlgItem::generatePixmap()
{
    if (!pLutFunc) return;
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

    if (!pVObj || !pVObj->histo() || pVObj->histo()->info()._count==0)
    {
        QPixmap pix(width(),height());
        pix.fill(Qt::red);
        QPainter pain(&pix);
        QPen pen(Qt::black);
        pain.drawText(QRectF(0,0,width(),height()),Qt::AlignCenter,"no input data");
        pain.end();
        setPixmap(pix);
    }
    else
    setPixmap(xVHistoDlg::bar(
                  xVHistoDlg::HDM_LUT,
                  size().width(),size().height(),
                  pVObj->histo()->info()._dataTypeMin,
                  pVObj->histo()->info()._dataTypeMax,
                  (*pVolVisPropObj->paramMap())["color function"]._value.value<vtkColorTransferFunctionPtr>()));
}

void xVLUTFuncDlgItem::mouseDoubleClickEvent(QMouseEvent *event)
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
    dlg.setMode(xVHistoDlg::HDM_LUT);
    dlg.setHisto(pVObj->histo());
    dlg.setColorTransFunc((*pVolVisPropObj->paramMap())["color function"]._value.value<vtkColorTransferFunctionPtr>());
    connect(&dlg,SIGNAL(modified(const QString&)),pVolVisPropObj,SLOT(paramModified(const QString&)));
    connect(&dlg,SIGNAL(KSignal(const SIG_TYPE& ,void *)),this,SIGNAL(KSignal(const SIG_TYPE& ,void *)));
    dlg.exec();
    generatePixmap();
}

void xVLUTFuncDlgItem::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    generatePixmap();
}
