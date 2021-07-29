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
    _dashboardLst[_currentDashBoard]->pPrevStepObj = nullptr;
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

xVObj_Basics *xVTKDlg::unprocessedObjCount(long& c)
{
    c=0;
    xVObj_Basics *unprocessed=nullptr;
    for (QList <xVAbstractBaseObj*>::iterator it2=_dashboardLst[_currentDashBoard]->_objLst.begin();it2!=_dashboardLst[_currentDashBoard]->_objLst.end();++it2)
    {
        xVObj_Basics *pVObj=dynamic_cast<xVObj_Basics*>(*it2);
        if (pVObj && pVObj->status()==OS_UPDATE_NEEDED) {
            c++;
            if (unprocessed==nullptr) unprocessed=pVObj;
        }
    }

    if (unprocessed!=nullptr) emit KSignal(ST_MSG,new QString(QString("unprocessd object %1 found").arg((*unprocessed->paramMap())["name"]._value.toString())));

    return unprocessed;
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
        _dashboardLst[_currentDashBoard]->pPrevStepObj=_dashboardLst[_currentDashBoard]->pCurrentStepObj;
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

            // check for error
            if (_dashboardLst[_currentDashBoard]->pCurrentStepObj && _dashboardLst[_currentDashBoard]->pCurrentStepObj->status()==OS_ERROR)
            {
                emit KSignal(ST_ERROR_MSG,new QString(QString("An error occured while running object %1. Program has been interrupted.").arg((*_dashboardLst[_currentDashBoard]->pCurrentStepObj->paramMap())["name"]._value.toString())));
                _dashboardLst[_currentDashBoard]->_inStepMode = false;
                ui->pStepTB->setEnabled(true);
                _dashboardLst[_currentDashBoard]->pCurrentStepObj=nullptr;
                _abort=true;
                return;
            }

            if (_dashboardLst[_currentDashBoard]->pCurrentStepObj->type()==xVOT_END)
            {
                //_dashboardLst[_currentDashBoard]->_passCount += 1;
                xVObj_Basics *pFirstUnprocessed = nullptr;
                long unpObjCount=0;
                pFirstUnprocessed=unprocessedObjCount(unpObjCount);
                if (unpObjCount>0)
                {
                    // check if their are unprocessed objects
                    emit KSignal(ST_MSG,new QString("unprocessed objects found ... starting an additional run"));

                    // go back to start obj
                    /*
                    for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
                    {
                        xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
                        if (pVObj && pVObj->type()==xVOT_START) _dashboardLst[_currentDashBoard]->pCurrentStepObj=pVObj;
                    }
                    */

                    _dashboardLst[_currentDashBoard]->pCurrentStepObj=pFirstUnprocessed;

                    // find prev element or start
                    bool _found = false;
                    xVObj_Basics *pUnprocessedChild=nullptr;
                    while (!_found && pFirstUnprocessed)
                    {
                        for (QList <xConnector*>::iterator it=pFirstUnprocessed->connectorLst()->begin();it!=pFirstUnprocessed->connectorLst()->end();++it)
                            if ((*it)->type()==xCT_INPUT)
                            {
                                for (QList <xVObj_Basics*>::iterator it2=(*it)->connectedObjects()->begin();it2!=(*it)->connectedObjects()->end();++it2)
                                {
                                    if (!_found && ((*it2)->status()==OS_VALID || (*it2)->type()==xVOT_START))
                                    {
                                        _found=true;
                                        _dashboardLst[_currentDashBoard]->pCurrentStepObj=*it2;

                                    }
                                    else
                                    {
                                        if ((*it2)->status()==OS_UPDATE_NEEDED) pUnprocessedChild=*it2;
                                    }
                                }
                            }

                        if (!_found) pFirstUnprocessed=pUnprocessedChild;
                    }

                    if (_dashboardLst[_currentDashBoard]->pCurrentStepObj!=nullptr)
                    {
                        emit KSignal(ST_MSG,new QString(QString("starting now %1").arg((*_dashboardLst[_currentDashBoard]->pCurrentStepObj->paramMap())["name"]._value.toString())));
                        for (QList <xConnector*>::iterator it4=_dashboardLst[_currentDashBoard]->pCurrentStepObj->connectorLst()->begin();
                             it4!=_dashboardLst[_currentDashBoard]->pCurrentStepObj->connectorLst()->end();++it4)
                            if ((*it4)->type()==xCT_OUTPUT)
                                _dashboardLst[_currentDashBoard]->_passCount = (*it4)->connectedObjects()->indexOf(pFirstUnprocessed);
                        emit KSignal(ST_MSG,new QString(QString("Index to follow is %1").arg(_dashboardLst[_currentDashBoard]->_passCount)));
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
                    {
                        int id = max(0,min((*it)->connectedObjects()->count()-1,(int)_dashboardLst[_currentDashBoard]->_passCount));
                        pParamObjFound=(*it)->connectedObjects()->at(id);
                    }

            _dashboardLst[_currentDashBoard]->pCurrentStepObj=pParamObjFound;
        //}
    }
    else
        _dashboardLst[_currentDashBoard]->_inStepMode = false;
    ui->pStepTB->setEnabled(true);
}
