#include "xVStartObj.h"

xVStartObj::xVStartObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_START;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _maxOutput=1000;

    generateShape();
    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(90-10,12.5);
    pGrpItem->addToGroup(pOutputCon->item());

    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVStartObj::xVStartObj(QDataStream& d):xVObj_Basics(d)
{
    _type = xVOT_START;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }

}

void xVStartObj::run()
{
    setStatus(OS_RUNNING);

    // calculate equation(s)

    setStatus(OS_VALID);
}

void xVStartObj::reset()
{

}

void xVStartObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-10,0);
    path.lineTo(50-20,0);
    path.lineTo(50-20,-12.5);
    path.lineTo(90-10,12.5);
    path.lineTo(50-20,25+12.5);
    path.lineTo(50-20,25);
    path.lineTo(-10,25);
    path.lineTo(0,12.5);
    path.lineTo(-10,0);

    QPen pen(Qt::black);
    QRadialGradient rg(-5,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(0,250,0));
    rg.setColorAt(1,QColor(255,255,255));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(80,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(-25,-15,140,45));

    pStatusItem->moveBy(5,0);
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
