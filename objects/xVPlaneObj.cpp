#include "xVPlaneObj.h"
#include "xVVolObj.h"
#include "vtkImagePlaneWidget.h"

xVPlaneObj::xVPlaneObj(const QString& txt):xVGenVisPropObj(txt)
{
    _type = xVOT_CLIPPING_PLANE;
}

xVPlaneObj::xVPlaneObj(QDataStream& d):xVGenVisPropObj(d)
{
    _type = xVOT_CLIPPING_PLANE;
}

void xVPlaneObj::run()
{
    xVGenVisPropObj::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        vtkImageData *pData = nullptr;
        for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    xVVolObj *pIObj = dynamic_cast<xVVolObj*>(*it);
                    if ((*it)->outputParamMap()->contains("volume data"))
                    {
                        pData =(*pIObj->outputParamMap())["volume data"]._value.value<vtkImageDataPtr>();
                    }

                }

        if (pData)
        {
            vtkImagePlaneWidget *pPlaneWdgt = vtkImagePlaneWidget::New();
        }

        setStatus(OS_VALID);
    }
}

void xVPlaneObj::paramModified(const QString& txt)
{
    xVGenVisPropObj::paramModified(txt);
}
