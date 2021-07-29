#include "xVDefuzzificationObj.h"

xVDefuzzificationObj::xVDefuzzificationObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_DEFUZZIFICATION;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    generateShape();
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

    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVDefuzzificationObj::xVDefuzzificationObj(QDataStream &d):xVObj_Basics(d)
{
    _type = xVOT_DEFUZZIFICATION;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}

void xVDefuzzificationObj::run()
{
    xVObj_Basics::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_VALID);
    }
}

void xVDefuzzificationObj::paramModified(const QString& txt)
{
    xVObj_Basics::paramModified(txt);

}

void xVDefuzzificationObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-10,0);
    for (int i=0;i<10;++i)
        path.cubicTo(QPointF(10+4*i,2+2*sin(M_PI/3*i)),
                     QPointF(10+4*i,2+2*sin(M_PI/3*(i+0.5))),
                     QPointF(10+4*(i+1),2+2*sin(M_PI/3*(i+1))));
    path.lineTo(110,0);
    path.lineTo(120,12.5);
    path.arcTo(QRectF(100,12.5,40,25),90,90);

    path.lineTo(65,25);
    for (int i=14;i>=0;--i)
        path.cubicTo(QPointF(10+4*i,23+2*sin(M_PI/3*i+0.5)),
                     QPointF(10+4*i,23+2*sin(M_PI/3*(i))),
                     QPointF(10+4*(i-1),23+2*sin(M_PI/3*(i-1))));
    path.lineTo(-10,25);
    path.lineTo(0,12.5);
    path.lineTo(-10,0);

    QPen pen(Qt::black);
    QRadialGradient rg(-5,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(250,250,0));
    rg.setColorAt(1,QColor(150,150,0));

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
