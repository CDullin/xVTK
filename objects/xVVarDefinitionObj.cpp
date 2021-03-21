#include "xVVarDefinitionObj.h"

xVVarDefinitionObj::xVVarDefinitionObj(QString txt):xVObj_Basics()
{
    _type = xVOT_VAR_DEFINITION;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["parameter table"]._id=2;
    _paramMp["parameter table"]._value=QVariant::fromValue(xParamMap());
    generateShape();
    _maxInput=1000;

    xConnector* pInputCon = new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(0,12.5);
    pGrpItem->addToGroup(pInputCon->item());
    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(120,12.5);
    pGrpItem->addToGroup(pOutputCon->item());
    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(52,-10);
    pGrpItem->addToGroup(pParamInputCon->item());

    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVVarDefinitionObj::xVVarDefinitionObj(QDataStream& d):xVObj_Basics(d)
{
    _type = xVOT_VAR_DEFINITION;
    generateShape();
    _maxInput=1000;
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
    // update global namespace
    /*
    for (QMap<QString,xPROP_TYPE>::iterator it=_paramMp["parameter table"]._value.value<xParamMap>().begin();
         it!=_paramMp["parameter table"]._value.value<xParamMap>().end();++it)
    {
        if (it->pRefObj==nullptr) // global namespace
        {
            _globalNameSpace[it.key()]=it.value();
        }
    }
    */
}
void xVVarDefinitionObj::run()
{
    xVObj_Basics::run();
    if (status()!=OS_UPDATE_NEEDED) return;
    setStatus(OS_RUNNING);
    // update global namespace
    xParamMap _cpy = _paramMp["parameter table"]._value.value<xParamMap>();
    for (QMap<QString,xPROP_TYPE>::iterator it=_cpy.begin();
         it!=_cpy.end();++it)
    {
        if (it->pRefObj==nullptr) // global namespace
        {
            _globalNameSpace[it.key()]=it.value();
        }
    }
    emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);
    setStatus(OS_VALID);
}
void xVVarDefinitionObj::save(QDataStream& d, bool _explicit)
{
    xVObj_Basics::save(d,_explicit);
}

void xVVarDefinitionObj::generateShape()
{
    QPainterPath path;
    path.addRoundedRect(QRectF(0,0,120,25),5,5);

    QPen pen(Qt::black);
    QRadialGradient rg(-5,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(250,250,0));
    rg.setColorAt(1,QColor(150,150,0));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(110,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(15,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(0,-15,150,45));

    pStatusItem->moveBy(15,0);
    pStatusItem->hide();
    pGrpItem->addToGroup(pStatusItem);

    pGrpItem->setFlags(QGraphicsItem::ItemIsMovable);
    pEffect=new QGraphicsDropShadowEffect();
    pEffect->setBlurRadius(5);
    pEffect->setOffset(QPointF(3,3));
    pGrpItem->setGraphicsEffect(pEffect);

    pGrpItem->setAcceptHoverEvents(true);
    pGrpItem->setAcceptedMouseButtons(Qt::AllButtons);
    pGrpItem->setHandlesChildEvents(false);
}
