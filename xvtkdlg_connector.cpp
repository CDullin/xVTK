#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVDashboardView.h"

void xVTKDlg::connectorActivated(xVObj_Basics* pEItem,xCONNECTOR_TYPE type)
{
    // self connection is forbidden
    if ((type==xCT_INPUT && _dashboardLst[_currentDashBoard]->pOutEItem && _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector() && pEItem==_dashboardLst[_currentDashBoard]->pOutEItem) ||
        (type==xCT_OUTPUT && _dashboardLst[_currentDashBoard]->pInEItem && _dashboardLst[_currentDashBoard]->pInEItem->activeConnector() && pEItem==_dashboardLst[_currentDashBoard]->pInEItem) ||
        (type==xCT_PARAMETER && _dashboardLst[_currentDashBoard]->pInEItem && _dashboardLst[_currentDashBoard]->pInEItem->activeConnector() && pEItem==_dashboardLst[_currentDashBoard]->pInEItem))
    {
        _dashboardLst[_currentDashBoard]->_paramInConnectorSet = false;
        _dashboardLst[_currentDashBoard]->_inConnectorSet = false;
        _dashboardLst[_currentDashBoard]->_outConnectorSet = false;

        if (_dashboardLst[_currentDashBoard]->pInEItem && _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()) _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()->setActivated(false);
        if (_dashboardLst[_currentDashBoard]->pOutEItem && _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()) _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()->setActivated(false);
        if (pEItem->activeConnector()) pEItem->activeConnector()->setActivated(false);

        _dashboardLst[_currentDashBoard]->pInEItem = nullptr;
        _dashboardLst[_currentDashBoard]->pOutEItem = nullptr;

        emit KSignal(ST_WARN_MSG,new QString("An object cannot be connected to itself"));

        return;
    }

    // 1st out->in check
    if (type==xCT_INPUT && pEItem->activeConnector() && pEItem->activeConnector()->type()==xCT_INPUT)
    {
        if (_dashboardLst[_currentDashBoard]->_inConnectorSet || _dashboardLst[_currentDashBoard]->_paramInConnectorSet)
            //deactivate the old one
            _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()->setActivated(false);
        _dashboardLst[_currentDashBoard]->pInEItem=pEItem;
        _dashboardLst[_currentDashBoard]->_paramInConnectorSet = false;
        _dashboardLst[_currentDashBoard]->_inConnectorSet = true;
    }
    if (type==xCT_INPUT && !pEItem->activeConnector() && _dashboardLst[_currentDashBoard]->_inConnectorSet)
    {
        _dashboardLst[_currentDashBoard]->_inConnectorSet=false;
        _dashboardLst[_currentDashBoard]->pInEItem=nullptr;
    }
    if (type==xCT_PARAMETER && pEItem->activeConnector() && pEItem->activeConnector()->type()==xCT_PARAMETER)
    {
        if (_dashboardLst[_currentDashBoard]->_inConnectorSet || _dashboardLst[_currentDashBoard]->_paramInConnectorSet)
            //deactivate the old one
            _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()->setActivated(false);
        _dashboardLst[_currentDashBoard]->pInEItem=pEItem;
        _dashboardLst[_currentDashBoard]->_inConnectorSet = false;
        _dashboardLst[_currentDashBoard]->_paramInConnectorSet = true;
    }
    if (type==xCT_PARAMETER && !pEItem->activeConnector() && _dashboardLst[_currentDashBoard]->_paramInConnectorSet)
    {
        _dashboardLst[_currentDashBoard]->_paramInConnectorSet=false;
        _dashboardLst[_currentDashBoard]->pInEItem=nullptr;
    }

    // 1st out->in check
    if (type==xCT_OUTPUT && pEItem->activeConnector() && pEItem->activeConnector()->type()==xCT_OUTPUT)
    {
        if (_dashboardLst[_currentDashBoard]->_outConnectorSet)
            //deactivate the old one
            _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()->setActivated(false);
        _dashboardLst[_currentDashBoard]->pOutEItem=pEItem;
        _dashboardLst[_currentDashBoard]->_outConnectorSet=true;
    }
    if (type==xCT_OUTPUT && !pEItem->activeConnector() && _dashboardLst[_currentDashBoard]->_outConnectorSet)
    {
        _dashboardLst[_currentDashBoard]->_outConnectorSet=false;
        _dashboardLst[_currentDashBoard]->pOutEItem=nullptr;
    }

    //2nd check if at anytime we have both
    if ((_dashboardLst[_currentDashBoard]->_inConnectorSet || _dashboardLst[_currentDashBoard]->_paramInConnectorSet) && _dashboardLst[_currentDashBoard]->_outConnectorSet)
    {
        if (_dashboardLst[_currentDashBoard]->_paramInConnectorSet)
        {
            _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()->addConObject(_dashboardLst[_currentDashBoard]->pOutEItem);
            //pInEItem->connectParamInputs()->append(pOutEItem);
            _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()->addConObject(_dashboardLst[_currentDashBoard]->pInEItem);
            //pOutEItem->connectOutputs()->append(pInEItem);
            xConnectorObj *pConObj = new xConnectorObj(_dashboardLst[_currentDashBoard]->pInEItem->activeConnector(),_dashboardLst[_currentDashBoard]->pOutEItem->activeConnector(),true);
            _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pConObj->item());
            emit KSignal(ST_ADD_OBJECT,pConObj);
            connect(_dashboardLst[_currentDashBoard]->pOutEItem,SIGNAL(parameterModified()),_dashboardLst[_currentDashBoard]->pInEItem,SLOT(paramModified()));
        }
        else
        {
            // reimplement max connection check
            if (_dashboardLst[_currentDashBoard]->pInEItem->countOfInputs()<_dashboardLst[_currentDashBoard]->pInEItem->maxInput() &&
                    _dashboardLst[_currentDashBoard]->pOutEItem->countOfOutputs()<_dashboardLst[_currentDashBoard]->pOutEItem->maxOutput())
            {
                _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()->addConObject(_dashboardLst[_currentDashBoard]->pOutEItem);
                _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()->addConObject(_dashboardLst[_currentDashBoard]->pInEItem);
                xConnectorObj *pConObj = new xConnectorObj(_dashboardLst[_currentDashBoard]->pInEItem->activeConnector(),_dashboardLst[_currentDashBoard]->pOutEItem->activeConnector());
                _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pConObj->item());
                emit KSignal(ST_ADD_OBJECT,pConObj);
                connect(_dashboardLst[_currentDashBoard]->pOutEItem,SIGNAL(parameterModified()),_dashboardLst[_currentDashBoard]->pInEItem,SLOT(paramModified()));
            }
            else emit KSignal(ST_WARN_MSG,new QString("Maximum connection count violated. Connection not established"));
        }

        if (_dashboardLst[_currentDashBoard]->pInEItem->activeConnector()) _dashboardLst[_currentDashBoard]->pInEItem->activeConnector()->setActivated(false);
        if (_dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()) _dashboardLst[_currentDashBoard]->pOutEItem->activeConnector()->setActivated(false);

        _dashboardLst[_currentDashBoard]->_inConnectorSet = false;
        _dashboardLst[_currentDashBoard]->_outConnectorSet = false;
        _dashboardLst[_currentDashBoard]->_paramInConnectorSet = false;
        _dashboardLst[_currentDashBoard]->pInEItem = nullptr;
        _dashboardLst[_currentDashBoard]->pOutEItem = nullptr;
    }
}
