#include "xVIFObj.h"
#include "xvEvalCondition.h"

xVIFObj::xVIFObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_IF;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["condition"]._value = QVariant::fromValue(xVEvalCondition("0"));
    _paramMp["condition"]._id = 1;
    generateShape();
    pYesOutputCon=new xConnector(this);
    _connectorLst.append(pYesOutputCon);
    pYesOutputCon->setToOutput();
    pYesOutputCon->item()->moveBy(52,22-9.5);
    pGrpItem->addToGroup(pYesOutputCon->item());
    connect(pYesOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    pNoOutputCon=new xConnector(this);
    _connectorLst.append(pNoOutputCon);
    pNoOutputCon->setToOutput();
    pNoOutputCon->item()->setRotation(90);
    pNoOutputCon->item()->moveBy(2,47-18);
    pGrpItem->addToGroup(pNoOutputCon->item());
    connect(pNoOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(2,-12.5);
    pGrpItem->addToGroup(pParamInputCon->item());
    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    xConnector* pInputCon=new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(-47,12.5);
    pGrpItem->addToGroup(pInputCon->item());
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVIFObj::xVIFObj(QDataStream& d):xVObj_Basics(d)
{
    _type = xVOT_IF;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
    pYesOutputCon=_connectorLst[0];
    pNoOutputCon=_connectorLst[1];
    pNoOutputCon->item()->setRotation(90);
}

void xVIFObj::reset()
{
    xVObj_Basics::reset();
    pYesOutputCon->setEnabled(true);
    pNoOutputCon->setEnabled(true);
    setStatus(OS_UPDATE_NEEDED);
}

void xVIFObj::run()
{
    xVObj_Basics::run();
    setStatus(OS_RUNNING);
    if (_paramMp["condition"]._value.value<xVEvalCondition>().toBool())
    {
        pYesOutputCon->setEnabled(true);
        pNoOutputCon->setEnabled(false);
    }
    else
    {
        pYesOutputCon->setEnabled(false);
        pNoOutputCon->setEnabled(true);
    }
    setStatus(OS_VALID);
}

void xVIFObj::generateShape()
{
    QPainterPath path;
    path.moveTo(2,-3);
    path.lineTo(52,22-9);
    path.lineTo(2,47-18);
    path.lineTo(-48,22-9);
    path.lineTo(2,-3);

    QPen pen(Qt::black);
    QRadialGradient rg(-5,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(250,250,0));
    rg.setColorAt(1,QColor(150,150,0));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(90,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(-23,15-9);

    pYesItem = new QGraphicsSimpleTextItem("yes");
    pYesItem->setPen(QPen(Qt::black));
    pYesItem->setPos(42,25-9);

    pNoItem = new QGraphicsSimpleTextItem("no");
    pNoItem->setPen(QPen(Qt::black));
    pNoItem->setPos(-18,42-18);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->addToGroup(pYesItem);
    pGrpItem->addToGroup(pNoItem);
    pGrpItem->setBoundingRectSize(QRectF(-50,-18,120,70));

    pStatusItem->setPos(32,-5);
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
