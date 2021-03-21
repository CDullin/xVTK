#include "xVEndObj.h"

xVEndObj::xVEndObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_END;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _maxOutput=0;
    _maxInput=1000;

    generateShape();
    xConnector* pCon=new xConnector(this);
    _connectorLst.append(pCon);
    pCon->setToInput();
    pCon->item()->moveBy(-5,12.5);
    pGrpItem->addToGroup(pCon->item());

    connect(pCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVEndObj::xVEndObj(QDataStream& d):xVObj_Basics(d)
{
    _type = xVOT_END;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }

}

void xVEndObj::run()
{
    xVObj_Basics::run();
    setStatus(OS_RUNNING);

    // calculate equation(s)

    setStatus(OS_VALID);
}

void xVEndObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-20,0-10-2.5);
    path.lineTo(-10,0-10-2.5);
    path.lineTo(10,5-2.5);
    path.lineTo(30,-10-2.5);
    path.lineTo(40,-10-2.5);
    path.lineTo(25,15-2.5);
    path.lineTo(40,30+10-2.5);
    path.lineTo(30,30+10-2.5);
    path.lineTo(10,25-2.5);
    path.lineTo(-10,30+10-2.5);
    path.lineTo(-20,30+10-2.5);
    path.lineTo(-5,15-2.5);
    path.lineTo(-20,-10-2.5);

    QPen pen(Qt::black);
    QRadialGradient rg(-5,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(250,0,0));
    rg.setColorAt(1,QColor(255,255,255));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(80,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(-7,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(-25,-15,140,45));

    pStatusItem->moveBy(-35,0);
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
