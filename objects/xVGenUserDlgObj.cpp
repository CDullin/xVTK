#include "xVGenUserDlgObj.h"

xVGenUserDlgObj::xVGenUserDlgObj(QDataStream &d):xVObj_Basics(d)
{
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}

xVGenUserDlgObj::xVGenUserDlgObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_USER_DLG;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id =0;
    generateShape();
    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(87,12.5);
    pGrpItem->addToGroup(pOutputCon->item());
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

void xVGenUserDlgObj::run()
{
    xVObj_Basics::run();
}

void xVGenUserDlgObj::generateShape()
{
    QPainterPath path;
    path.moveTo(5,0);
    path.lineTo(67,0);
    path.arcTo(QRectF(67-20,0,40,25),90,-180);
    path.lineTo(0,25);
    path.lineTo(5,0);
    path.moveTo(67,5);
    path.arcTo(QRectF(67-15,5,30,15),90,-180);
    path.lineTo(67,5);

    QPen pen(Qt::black);
    QRadialGradient rg(10,10, 100, 0, 1, 5);
    rg.setColorAt(0,QColor(255,255,255));
    rg.setColorAt(1,QColor(100,100,100));

    QBrush brush(rg);
    pShapeItem = new xGraphicsPathItem(path);
    connect(pShapeItem,SIGNAL(hoverEnter()),this,SLOT(updateDescToolTip()));
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(80,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(15,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setFlags(QGraphicsItem::ItemIsMovable);
    pEffect=new QGraphicsDropShadowEffect();
    pEffect->setBlurRadius(5);
    pEffect->setOffset(QPointF(3,3));
    pGrpItem->setGraphicsEffect(pEffect);
    pGrpItem->setAcceptHoverEvents(true);
    pGrpItem->setAcceptedMouseButtons(Qt::AllButtons);
    pGrpItem->setHandlesChildEvents(false);
    pGrpItem->setBoundingRectSize(QRectF(0,0,125,30));
/*
    pBoundingRectItem = new QGraphicsRectItem(pGrpItem->boundingRect());
    pBoundingRectItem->setPen(QPen(Qt::white,2));
    pBoundingRectItem->setBrush(QBrush(Qt::NoBrush));
    pGrpItem->addToGroup(pBoundingRectItem);
    */
}
