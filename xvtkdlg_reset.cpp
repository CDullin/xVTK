#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"

#include <QMessageBox>

void recursiveRest(xVObj_Basics* pObj)
{
    if (pObj && pObj->status()!=OS_UPDATE_NEEDED)
    {
        pObj->reset();
        for (QList<xConnector*>::iterator it2=pObj->connectorLst()->begin(); it2!=pObj->connectorLst()->end(); ++it2) {
            if ((*it2)->type()!=xCT_OUTPUT)
            {
                for (QList<xVObj_Basics*>::iterator it3=(*it2)->connectedObjects()->begin();it3!=(*it2)->connectedObjects()->end();++it3)
                    recursiveRest(*it3);
            }
        }
    }
}

void xVTKDlg::reset(bool verbose)
{
    if (verbose && QMessageBox::question(0,"xVTK - question","Resetting the dashboard will result "
                                        "in destroying all visualization windows and removing all data. "
                                        "Do you really like to do that?")==QMessageBox::No) return;

    // find all end points / visulizations
    QList <xVObj_Basics*> _endPointLst;
    for (QList<xVAbstractBaseObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
    {
        xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
        // we take all those that have no output
        if (pVObj)
        {
            int outputCount=0;
            for (QList<xConnector*>::iterator it2=pVObj->connectorLst()->begin(); it2!=pVObj->connectorLst()->end(); ++it2) {
                if ((*it2)->type()==xCT_OUTPUT) outputCount++;
            }
            if (outputCount==0)
                _endPointLst.append(pVObj);
        }
    }

    // first we reset recursively all objects that are endpoints
    for (QList<xVObj_Basics*>::iterator it=_endPointLst.begin();it!=_endPointLst.end() ; ++it) {
        recursiveRest(*it);
    }
    // then we reset all remaining objects
    for (QList<xVAbstractBaseObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
    {
        xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
        // we take all those that have no output
        if (pVObj && pVObj->status()!=OS_UPDATE_NEEDED)
            pVObj->reset();
    }
}
