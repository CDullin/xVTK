#include "xvpropwdgt.h"
#include "xVDashboardView.h"
#include "ui_xvpropwdgt.h"
#include <QVariant>
#include "xVCustomTableItems.h"
#include "xVTypes.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QGraphicsScene>
#include "xVVolumeVisPropObj.h"
#include "xviconfactory.h"

xVPropWdgt::xVPropWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVPropWdgt)
{
    ui->setupUi(this);
    connect(ui->pDeleteTB,SIGNAL(clicked()),this,SLOT(deleteObj()));
    connect(ui->pPreviewTB,SIGNAL(clicked()),this,SLOT(displayPreview()));
    connect(ui->pPropTable,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SIGNAL(KSignal(const SIG_TYPE&,void *)));
    connect(ui->pConnectionLWdgt,SIGNAL(itemSelectionChanged()),this,SLOT(connectionSelectionChanged()));
    connect(ui->pDelConnectionTB,SIGNAL(clicked()),this,SLOT(deleteConnection()));
    connect(ui->pUpConnectionTB,SIGNAL(clicked()),this,SLOT(moveConnectionUp()));
    connect(ui->pDownConnectionTB,SIGNAL(clicked()),this,SLOT(moveConnectionDown()));
    connect(ui->pIdentifyTB,SIGNAL(toggled(bool)),this,SLOT(identifyConnections(bool)));
    connect(ui->pNextObjTB,SIGNAL(clicked()),this,SLOT(activateNext()));
    connect(ui->pPrevObjTB,SIGNAL(clicked()),this,SLOT(activatePrev()));
}

void xVPropWdgt::activateNext()
{
    emit KSignal(ST_ACTIVATE_NEXT_OBJ);
}

void xVPropWdgt::activatePrev()
{
    emit KSignal(ST_ACTIVATE_PREV_OBJ);
}

void xVPropWdgt::installEventFilter(QObject *obj)
{
    ui->pPropTable->installEventFilter(obj);
    QObject::installEventFilter(obj);
}

void xVPropWdgt::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type) {
    case ST_OBJ_ADDED_TO_CONNECTOR:
    case ST_OBJ_REMOVED_FROM_CONNECTOR:
        if (pCurrentOutputConnector == (xConnector*)pData)
        {
            updateOutputConnectionList(pCurrentObj);
            connectionSelectionChanged();
            identifyConnections(ui->pIdentifyTB->isChecked());
        }
        break;
    case ST_OBJECT_REMOVED: if (pCurrentObj==(xVAbstractBaseObj*)pData)
        {
            pCurrentObj=nullptr;
            hide();
        }
        break;
    default:
        // signal not needed
        break;
    }
    // call table directly
    ui->pPropTable->KSlot(type,pData);
}

void xVPropWdgt::deleteObj()
{
    if (pCurrentObj && QMessageBox::question(0,"Warning!","Do you really like to destroy this object?")==QMessageBox::Yes)
    {
        emit KSignal(ST_REMOVE_OBJECT,pCurrentObj);
        pCurrentObj=nullptr;
        hide();
    }
}

void xVPropWdgt::displayPreview()
{
    if (pCurrentObj && pCurrentObj->hasPreview())
        pCurrentObj->openPreview();
}

xVPropWdgt::~xVPropWdgt()
{
    delete ui;
}

void xVPropWdgt::objSelected(xVAbstractBaseObj* pObj)
{

    if (pCurrentObj!=nullptr) pCurrentObj->setParamSelected(false);
    if (pCurrentObj==pObj) {
        pObj->setParamSelected(false);
        hide();
        pCurrentObj=nullptr;
        return;
    }

    updateParameterTable(pObj);
    updateOutputConnectionList(pObj);
    connectionSelectionChanged();
    identifyConnections(ui->pIdentifyTB->isChecked());

    ui->pDeleteTB->setEnabled(pObj && pObj->type()!=xVOT_START && pObj->type()!=xVOT_END);
    ui->pPreviewTB->setEnabled(pObj && pObj->hasPreview());
}

void xVPropWdgt::parameterModified()
{
    // close external dialogs

    /*
    xAbstractBasisObj* pObj=dynamic_cast<xAbstractBasisObj*>(sender());
    if (pObj && pObj==pCurrentObj)
    {
        int cr=ui->pPropTable->verticalScrollBar()->value();
        updateParameterTable(pCurrentObj);
        ui->pPropTable->verticalScrollBar()->setValue(cr);
    }
    */
}

void xVPropWdgt::connectionSelectionChanged()
{
    ui->pDelConnectionTB->setEnabled(false);
    ui->pUpConnectionTB->setEnabled(false);
    ui->pDownConnectionTB->setEnabled(false);

    long _r=-1;
    QList<QTableWidgetSelectionRange> _sel=ui->pConnectionLWdgt->selectedRanges();
    if (_sel.count()>0) _r=_sel[0].topRow();

    ui->pDelConnectionTB->setEnabled(_r!=-1);
    ui->pUpConnectionTB->setEnabled(_r>0);
    ui->pDownConnectionTB->setEnabled(_r<ui->pConnectionLWdgt->rowCount()-1 && _r>-1);
}

void xVPropWdgt::deleteConnection()
{
    QList<QTableWidgetSelectionRange> _sel=ui->pConnectionLWdgt->selectedRanges();
    int _r=-1;
    if (_sel.count()>0) _r=_sel[0].topRow();
    if (_r==-1) return;

    // identity connection object
    xVAbstractBaseObj *pKillObj=nullptr;
    for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
    {
        xConnectorObj *pConObj = dynamic_cast<xConnectorObj*>(*it);
        if (pConObj && pConObj->outputObj()->baseObj()==pCurrentObj &&
                pConObj->inputObj()->baseObj()==(xVObj_Basics*)(ui->pConnectionLWdgt->item(_r,0)->data(QVariant::UserType).toULongLong()))
            pKillObj=*it;
    }
    if (pKillObj!=nullptr)
    {
        emit KSignal(ST_REMOVE_OBJECT,pKillObj);
        updateOutputConnectionList(pCurrentObj);
        connectionSelectionChanged();
    }

    identifyConnections(ui->pIdentifyTB->isChecked());
}

void xVPropWdgt::moveConnectionUp()
{
    QList<QTableWidgetSelectionRange> _sel=ui->pConnectionLWdgt->selectedRanges();
    int _r=-1;
    if (_sel.count()>0) _r=_sel[0].topRow();
    if (_r==-1) return;

    if (pCurrentOutputConnector)
    {
        pCurrentOutputConnector->connectedObjects()->swapItemsAt(_r,_r-1);
        updateOutputConnectionList(pCurrentObj);
        connectionSelectionChanged();
        ui->pConnectionLWdgt->selectRow(_r-1);
    }

    identifyConnections(ui->pIdentifyTB->isChecked());
}

void xVPropWdgt::moveConnectionDown()
{
    QList<QTableWidgetSelectionRange> _sel=ui->pConnectionLWdgt->selectedRanges();
    int _r=-1;
    if (_sel.count()>0) _r=_sel[0].topRow();
    if (_r==-1) return;

    if (pCurrentOutputConnector)
    {
        pCurrentOutputConnector->connectedObjects()->swapItemsAt(_r,_r+1);
        updateOutputConnectionList(pCurrentObj);
        connectionSelectionChanged();
        ui->pConnectionLWdgt->selectRow(_r+1);
    }

    identifyConnections(ui->pIdentifyTB->isChecked());
}

void xVPropWdgt::identifyConnections(bool b)
{
    // clear all identfiers
    for (int i=0;i<_identifierLst.count();++i) {
        if (_dashboardLst[_currentDashBoard]->pDashBoardGV && _dashboardLst[_currentDashBoard]->pDashBoardGV->scene())
            _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->removeItem(_identifierLst[i]);
        delete (_identifierLst[i]);
    }
    _identifierLst.clear();

    if (b)
    {
        for (int r=0;r<ui->pConnectionLWdgt->rowCount();++r)
        {
            xVObj_Basics *pVObj = (xVObj_Basics *)ui->pConnectionLWdgt->item(r,0)->data(QVariant::UserType).toULongLong();
            QGraphicsSimpleTextItem *pItem= new QGraphicsSimpleTextItem(QString("%1").arg(r+1));
            QPen p(Qt::white,2);
            QBrush br(Qt::white);
            QFont f=font();
            f.setPixelSize(20);
            pItem->setFont(f);
            pItem->setPen(p);
            pItem->setBrush(br);
            _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pItem);
            _identifierLst.append(pItem);
            pItem->setPos(pVObj->item()->pos()-QPointF(5,15));
            pItem->setZValue(100);
            QGraphicsDropShadowEffect *pEffect = new QGraphicsDropShadowEffect();
            pEffect->setColor(Qt::black);
            pEffect->setOffset(2,2);
            pEffect->setBlurRadius(3);
            pItem->setGraphicsEffect(pEffect);
        }
    }
}

void xVPropWdgt::updateOutputConnectionList(xVAbstractBaseObj* pObj)
{
    xVObj_Basics *pVObj=dynamic_cast<xVObj_Basics*>(pObj);
    bool _found=false;

    if (pVObj)
    {
        ui->pConnectionLWdgt->clear();
        int r=0;
        ui->pConnectionLWdgt->setColumnCount(1);
        ui->pConnectionLWdgt->setRowCount(0);

        for (QList <xConnector*>::iterator it=pVObj->connectorLst()->begin();it!=pVObj->connectorLst()->end();++it)
        {
            if ((*it)->type()==xCT_OUTPUT)                
            {
                pCurrentOutputConnector = (*it);
                for (QList <xVObj_Basics*>::iterator it2=(*it)->connectedObjects()->begin();it2!=(*it)->connectedObjects()->end();++it2)
                {
                    _found = true;
                    bool _input = false;
                    ui->pConnectionLWdgt->setRowCount(r+1);
                    for (QList <xConnector*>::iterator it3=(*it2)->connectorLst()->begin();it3!=(*it2)->connectorLst()->end();++it3)
                        if ((*it3)->type()==xCT_INPUT && (*it3)->connectedTo(pVObj)) _input=true;

                    QTableWidgetItem *pItem = new QTableWidgetItem((*(*it2)->paramMap())["name"]._value.toString());
                    pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
                    pItem->setIcon(_input ? _iconFactory.icon("input connector") : _iconFactory.icon("parmeter output connector"));
                    pItem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
                    pItem->setData(QVariant::UserType,QVariant((quint64)(*it2)));
                    ui->pConnectionLWdgt->setItem(r,0,pItem);
                    r++;
                }
            }
        }
        ui->pConnectionLWdgt->resizeColumnToContents(0);
    }
    ui->pOutputConGB->setEnabled(_found);
}

void xVPropWdgt::updateParameterTable(xVAbstractBaseObj* pObj)
{
    qApp->processEvents();
    pCurrentObj=pObj;
    pObj->setParamSelected(true);
    ui->pPropTable->updateTable(pCurrentObj->paramMap(),pCurrentObj);
    show();
}
