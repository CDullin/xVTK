#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVObjectTypes.h"
#include <stdlib.h>

using namespace std;

/*
 * always start at startObj
 * follow forward connection until meaning take the first object of the connector list
 * check if it needs an update
 * if yes run it
 * else if no error status then simple progress through it
 *
 * stop if a
 * a) dead end is reached
 * b) an object needs more than one input -> start an startObj again
 *
 * logic objects apart definition objects are executed every time
 */



void xVTKDlg::run()
{
    bool force=true;
    do
    {
        step(force);
        force=false;
    }while(_inStepMode);
}

long unprocessedObjCount()
{
    long c=0;
    for (QList <xVAbstractBaseObj*>::iterator it2=_objLst.begin();it2!=_objLst.end();++it2)
    {
        xVObj_Basics *pVObj=dynamic_cast<xVObj_Basics*>(*it2);
        if (pVObj && pVObj->status()==OS_UPDATE_NEEDED) c++;
    }
    return c;
}

void xVTKDlg::step(bool _force)
{
    ui->pStepTB->setEnabled(false);
    if (_force || !_inStepMode)
    {
        reset(false);
        _inStepMode = true;
        _passCount = 0;
        for (QList <xVAbstractBaseObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
        {
            xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
            if (pVObj && pVObj->type()==xVOT_START) pCurrentStepObj=pVObj;
        }
    }
    if (pCurrentStepObj)
    {
        // run current obj
        if (pCurrentStepObj->status()==OS_UPDATE_NEEDED || unprocessedObjCount()>0 || (pCurrentStepObj->type() & 0x00020000) > 0)
        {
            // that should give us all parameter objects first
            xVObj_Basics *pParamObjFound=nullptr;
            for (QList <xConnector*>::iterator it = pCurrentStepObj->connectorLst()->begin();it!=pCurrentStepObj->connectorLst()->end();++it)
                if ((*it)->type()==xCT_PARAMETER && (*it)->isEnabled())
                    for (QList <xVObj_Basics*>::iterator it2=(*it)->connectedObjects()->begin();it2!=(*it)->connectedObjects()->end();++it2)
                        if ((*it2)->status()==OS_UPDATE_NEEDED)
                            pParamObjFound=(*it2);

            if (pParamObjFound) pCurrentStepObj=pParamObjFound;
            pCurrentStepObj->run();
            if (pCurrentStepObj->type()==xVOT_END)
            {
                _passCount += 1;
                if (unprocessedObjCount()>0)
                {
                    // check if their are unprocessed objects
                    emit KSignal(ST_MSG,new QString("unprocessed objects found ... starting an additional run"));
                    // go back to start obj
                    for (QList <xVAbstractBaseObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
                    {
                        xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
                        if (pVObj && pVObj->type()==xVOT_START) pCurrentStepObj=pVObj;
                    }
                }
                else
                    emit KSignal(ST_MSG,new QString("run completed ..."));
            }
            // foward one ... we take always the first output object
            pParamObjFound=nullptr;
            for (QList <xConnector*>::iterator it = pCurrentStepObj->connectorLst()->begin();it!=pCurrentStepObj->connectorLst()->end();++it)
                if ((*it)->type()==xCT_OUTPUT && (*it)->isEnabled())
                    if ((*it)->connectedObjects()->count()>0)
                        pParamObjFound=(*it)->connectedObjects()->at(min((*it)->connectedObjects()->count()-1,(int)_passCount));

            pCurrentStepObj=pParamObjFound;
        }
    }
    else
        _inStepMode = false;
    ui->pStepTB->setEnabled(true);
}
