#include "xVGenFilterObj.h"

xVGenFilterObj::xVGenFilterObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_FILTER;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    generateShape();

    xConnector*pInputCon = new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(0,12.5);
    pGrpItem->addToGroup(pInputCon->item());

    xConnector*pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(120,12.5);
    pGrpItem->addToGroup(pOutputCon->item());
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));

    xConnector*pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(50,-10);
    pGrpItem->addToGroup(pParamInputCon->item());
    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));

    _inputRequirements << (QStringList() << "image");
    _inputRequirements << (QStringList() << "volume");
}

xVGenFilterObj::xVGenFilterObj(QDataStream &d):xVObj_Basics(d)
{
    _type = xVOT_FILTER;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}

void xVGenFilterObj::run()
{
    xVObj_Basics::run();
}

void xVGenFilterObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-10,0);
    path.lineTo(110,0);
    path.lineTo(120,12.5);
    path.lineTo(110,25);
    path.lineTo(-10,25);
    path.lineTo(0,12.5);
    path.lineTo(-10,0);

    QPen pen(Qt::black);
    QRadialGradient rg(10,10, 100, 0, 1, 5);
    rg.setColorAt(0,QColor(100,100,100));
    rg.setColorAt(1,QColor(200,200,200));

    QBrush brush(rg);
    pShapeItem = new xGraphicsPathItem(path);
    connect(pShapeItem,SIGNAL(hoverEnter()),this,SLOT(updateDescToolTip()));
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(130,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(-25,-15,167,45));
/*
    pBoundingRectItem = new QGraphicsRectItem(pGrpItem->boundingRect());
    pBoundingRectItem->setPen(QPen(Qt::white,2));
    pBoundingRectItem->setBrush(QBrush(Qt::NoBrush));
    pGrpItem->addToGroup(pBoundingRectItem);
*/
    pStatusItem->moveBy(5,0);
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
