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

void xVTKDlg::stop()
{
    _abort=true;
}

void xVTKDlg::run()
{
    _abort = false;
    ui->pStopTB->setEnabled(true);
    bool force=true;
    do
    {
        step(force);
        force=false;
    }while(_dashboardLst[_currentDashBoard]->_inStepMode && !_abort);
    ui->pStopTB->setEnabled(false);
    emit KSignal(ST_SET_ALL_PROCESS_RANGE,new QPoint(0,100));
    emit KSignal(ST_SET_PROCESS_RANGE,new QPoint(0,100));
    emit KSignal(ST_SET_PROCESS,new int(100));
    emit KSignal(ST_SET_ALL_PROCESS,new int(100));
    emit KSignal(ST_SET_PROCESS_TXT,new QString("done"));
    emit KSignal(ST_SET_ALL_PROCESS_TXT,new QString("done"));
}

long unprocessedObjCount()
{
    long c=0;
    for (QList <xVAbstractBaseObj*>::iterator it2=_dashboardLst[_currentDashBoard]->_objLst.begin();it2!=_dashboardLst[_currentDashBoard]->_objLst.end();++it2)
    {
        xVObj_Basics *pVObj=dynamic_cast<xVObj_Basics*>(*it2);
        if (pVObj && pVObj->status()==OS_UPDATE_NEEDED) c++;
    }
    return c;
}

void xVTKDlg::step(bool _force)
{
    ui->pStepTB->setEnabled(false);
    if (_force || !_dashboardLst[_currentDashBoard]->_inStepMode)
    {
        reset(false);
        _dashboardLst[_currentDashBoard]->_inStepMode = true;
        _dashboardLst[_currentDashBoard]->_passCount = 0;
        for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
        {
            xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
            if (pVObj && pVObj->type()==xVOT_START) _dashboardLst[_currentDashBoard]->pCurrentStepObj=pVObj;
        }
    }
    if (_dashboardLst[_currentDashBoard]->pCurrentStepObj)
    {
        // run current obj
        //if (pCurrentStepObj->status()==OS_UPDATE_NEEDED || unprocessedObjCount()>0 || (pCurrentStepObj->type() & 0x00020000) > 0)
        //{
            // that should give us all parameter objects first
            xVObj_Basics *pParamObjFound=nullptr;
            for (QList <xConnector*>::iterator it = _dashboardLst[_currentDashBoard]->pCurrentStepObj->connectorLst()->begin();it!=_dashboardLst[_currentDashBoard]->pCurrentStepObj->connectorLst()->end();++it)
                if ((*it)->type()==xCT_PARAMETER && (*it)->isEnabled())
                    for (QList <xVObj_Basics*>::iterator it2=(*it)->connectedObjects()->begin();it2!=(*it)->connectedObjects()->end();++it2)
                        if ((*it2)->status()==OS_UPDATE_NEEDED)
                            pParamObjFound=(*it2);

            if (pParamObjFound) _dashboardLst[_currentDashBoard]->pCurrentStepObj=pParamObjFound;
            _dashboardLst[_currentDashBoard]->pCurrentStepObj->run();
            if (_dashboardLst[_currentDashBoard]->pCurrentStepObj->type()==xVOT_END)
            {
                _dashboardLst[_currentDashBoard]->_passCount += 1;
                if (unprocessedObjCount()>0)
                {
                    // check if their are unprocessed objects
                    emit KSignal(ST_MSG,new QString("unprocessed objects found ... starting an additional run"));
                    // go back to start obj
                    for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
                    {
                        xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
                        if (pVObj && pVObj->type()==xVOT_START) _dashboardLst[_currentDashBoard]->pCurrentStepObj=pVObj;
                    }
                }
                else
                {
                    emit KSignal(ST_MSG,new QString("run completed ..."));
                    _dashboardLst[_currentDashBoard]->pCurrentStepObj=nullptr;
                    ui->pStepTB->setEnabled(true);
                    return;
                }
            }
            // foward one ... we take always the first output object
            pParamObjFound=nullptr;
            for (QList <xConnector*>::iterator it = _dashboardLst[_currentDashBoard]->pCurrentStepObj->connectorLst()->begin();it!=_dashboardLst[_currentDashBoard]->pCurrentStepObj->connectorLst()->end();++it)
                if ((*it)->type()==xCT_OUTPUT && (*it)->isEnabled())
                    if ((*it)->connectedObjects()->count()>0)
                        pParamObjFound=(*it)->connectedObjects()->at(min((*it)->connectedObjects()->count()-1,(int)_dashboardLst[_currentDashBoard]->_passCount));

            _dashboardLst[_currentDashBoard]->pCurrentStepObj=pParamObjFound;
        //}
    }
    else
        _dashboardLst[_currentDashBoard]->_inStepMode = false;
    ui->pStepTB->setEnabled(true);
}
