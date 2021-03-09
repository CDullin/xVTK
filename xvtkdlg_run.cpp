#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVObjectTypes.h"

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


void xVTKDlg::processRec(xVObj_Basics* pVObj)
{
    if (pVObj)
    {
        for (QList <xConnector*>::iterator it2=pVObj->connectorLst()->begin();it2!=pVObj->connectorLst()->end();++it2)
        {
            // find all connected parameter input dlgs
            if ((*it2)->type()==xCT_PARAMETER && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                    if ((*it)->status()==OS_UPDATE_NEEDED) processRec(*it);
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                    if ((*it)->status()==OS_UPDATE_NEEDED) processRec(*it);
        }
        if (pVObj->status()==OS_UPDATE_NEEDED) pVObj->run();
        if (pVObj->status()!=OS_ERROR) emit KSignal(ST_INC_ALL_PROCESS);
        for (QList <xConnector*>::iterator it2=pVObj->connectorLst()->begin();it2!=pVObj->connectorLst()->end();++it2)
            // find all connected and enabled outputs
            if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                    if ((*it)->status()==OS_UPDATE_NEEDED) processRec(*it);
    }
}

void processFwd(xVObj_Basics *pVObj)
{
    if (pVObj)
    {
        // find all connected parameter input dlgs
        for (QList <xConnector*>::iterator it2=pVObj->connectorLst()->begin();it2!=pVObj->connectorLst()->end();++it2)
        {
            if ((*it2)->type()==xCT_PARAMETER && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                    if ((*it)->status()==OS_UPDATE_NEEDED) processFwd(*it);
        }
        pVObj->run();
        // find 1st unprocessed connected output obj
        xVObj_Basics* pNextObj=nullptr;
        for (QList <xConnector*>::iterator it2=pVObj->connectorLst()->begin();it2!=pVObj->connectorLst()->end();++it2)
        {
            if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                    if ((*it)->status()==OS_UPDATE_NEEDED && pNextObj==nullptr) pNextObj=(*it);
        }
        processFwd(pNextObj);
    }
}

void xVTKDlg::run()
{
    reset(false);
    emit KSignal(ST_MSG,new QString("start processing ..."));
    QList <xVObj_Basics*> runLst;
    long objCount=0;
    xVStartObj *pSObj=nullptr;
    // set status of all objects to in process
    for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
    {
        xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
        //xVGenImpObj* pIObj = dynamic_cast<xVGenImpObj*>(*it);
        if (pVObj)
        {
            ++objCount;
            pVObj->setStatus(OS_UPDATE_NEEDED);
            if (pVObj->type() & xVOT_DATA) runLst.append(pVObj);
        }

        xVStartObj* pTSObj = dynamic_cast<xVStartObj*>(*it);
        if (pTSObj) pSObj=pTSObj;
    }

    long _unprocessedObjCount=1;
    while (_unprocessedObjCount>0)
    {
        _unprocessedObjCount=0;
        processFwd(pSObj);
        for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
        {
            xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
            if (pVObj && pVObj->status()==OS_UPDATE_NEEDED) ++_unprocessedObjCount;
        }
    }
    /*

    // process importer and proceed forward
    if (runLst.count()>0)
    {
        emit KSignal(ST_SET_ALL_PROCESS_RANGE,new QPoint(0,objCount-1));
        emit KSignal(ST_SET_ALL_PROCESS,new int(0));
        bool _error=false;
        for (QList <xVObj_Basics*>::iterator it=runLst.begin();it!=runLst.end();++it)
        {
            emit KSignal(ST_RESET_PROCESS);
            processRec(*it);
            qApp->processEvents();
            emit KSignal(ST_SET_PROCESS,new int(0));
        }
        emit KSignal(ST_RESET_PROCESS);
    }
    else emit KSignal(ST_WARN_MSG,new QString("no import objects found"));

    */
}
