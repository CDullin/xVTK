#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"

void xVTKDlg::connectorActivated(xVObj_Basics* pEItem,xCONNECTOR_TYPE type)
{
    // self connection is forbidden
    if ((type==xCT_INPUT && pOutEItem && pOutEItem->activeConnector() && pEItem==pOutEItem) ||
        (type==xCT_OUTPUT && pInEItem && pInEItem->activeConnector() && pEItem==pInEItem) ||
        (type==xCT_PARAMETER && pInEItem && pInEItem->activeConnector() && pEItem==pInEItem))
    {
        _paramInConnectorSet = false;
        _inConnectorSet = false;
        _outConnectorSet = false;

        if (pInEItem && pInEItem->activeConnector()) pInEItem->activeConnector()->setActivated(false);
        if (pOutEItem && pOutEItem->activeConnector()) pOutEItem->activeConnector()->setActivated(false);
        if (pEItem->activeConnector()) pEItem->activeConnector()->setActivated(false);

        pInEItem = nullptr;
        pOutEItem = nullptr;

        emit KSignal(ST_WARN_MSG,new QString("An object cannot be connected to itself"));

        return;
    }

    // 1st out->in check
    if (type==xCT_INPUT && pEItem->activeConnector() && pEItem->activeConnector()->type()==xCT_INPUT)
    {
        if (_inConnectorSet || _paramInConnectorSet)
            //deactivate the old one
            pInEItem->activeConnector()->setActivated(false);
        pInEItem=pEItem;
        _paramInConnectorSet = false;
        _inConnectorSet = true;
    }
    if (type==xCT_INPUT && !pEItem->activeConnector() && _inConnectorSet)
    {
        _inConnectorSet=false;
        pInEItem=nullptr;
    }
    if (type==xCT_PARAMETER && pEItem->activeConnector() && pEItem->activeConnector()->type()==xCT_PARAMETER)
    {
        if (_inConnectorSet || _paramInConnectorSet)
            //deactivate the old one
            pInEItem->activeConnector()->setActivated(false);
        pInEItem=pEItem;
        _inConnectorSet = false;
        _paramInConnectorSet = true;
    }
    if (type==xCT_PARAMETER && !pEItem->activeConnector() && _paramInConnectorSet)
    {
        _paramInConnectorSet=false;
        pInEItem=nullptr;
    }

    // 1st out->in check
    if (type==xCT_OUTPUT && pEItem->activeConnector() && pEItem->activeConnector()->type()==xCT_OUTPUT)
    {
        if (_outConnectorSet)
            //deactivate the old one
            pOutEItem->activeConnector()->setActivated(false);
        pOutEItem=pEItem;
        _outConnectorSet=true;
    }
    if (type==xCT_OUTPUT && !pEItem->activeConnector() && _outConnectorSet)
    {
        _outConnectorSet=false;
        pOutEItem=nullptr;
    }

    //2nd check if at anytime we have both
    if ((_inConnectorSet || _paramInConnectorSet) && _outConnectorSet)
    {
        if (_paramInConnectorSet)
        {
            pInEItem->activeConnector()->addConObject(pOutEItem);
            //pInEItem->connectParamInputs()->append(pOutEItem);
            pOutEItem->activeConnector()->addConObject(pInEItem);
            //pOutEItem->connectOutputs()->append(pInEItem);
            xConnectorObj *pConObj = new xConnectorObj(pInEItem->activeConnector(),pOutEItem->activeConnector(),true);
            ui->pDashBoardGV->scene()->addItem(pConObj->item());
            emit KSignal(ST_ADD_OBJECT,pConObj);
            connect(pOutEItem,SIGNAL(parameterModified()),pInEItem,SLOT(paramModified()));
        }
        else
        {
            // reimplement max connection check
            if (pInEItem->countOfInputs()<pInEItem->maxInput() && pOutEItem->countOfOutputs()<pOutEItem->maxOutput())
            {
                pInEItem->activeConnector()->addConObject(pOutEItem);
                pOutEItem->activeConnector()->addConObject(pInEItem);
                xConnectorObj *pConObj = new xConnectorObj(pInEItem->activeConnector(),pOutEItem->activeConnector());
                ui->pDashBoardGV->scene()->addItem(pConObj->item());
                emit KSignal(ST_ADD_OBJECT,pConObj);
                connect(pOutEItem,SIGNAL(parameterModified()),pInEItem,SLOT(paramModified()));
            }
            else emit KSignal(ST_WARN_MSG,new QString("Maximum connection count violated. Connection not established"));
        }

        if (pInEItem->activeConnector()) pInEItem->activeConnector()->setActivated(false);
        if (pOutEItem->activeConnector()) pOutEItem->activeConnector()->setActivated(false);

        _inConnectorSet = false;
        _outConnectorSet = false;
        _paramInConnectorSet = false;
        pInEItem = nullptr;
        pOutEItem = nullptr;
    }
}
