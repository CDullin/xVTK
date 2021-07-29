#include "xVMathObj.h"
#include "xvEvalCondition.h"

xVMathObj::xVMathObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_MATH;
    _description="Calculates mathematical equations";
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["equation"]._value = QVariant::fromValue(xVEvalCondition(""));
    _paramMp["equation"]._id = 3;
    _maxInput=1000;

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

    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVMathObj::xVMathObj(QDataStream& d):xVObj_Basics(d)
{
    _maxInput=1000;
    _type = xVOT_MATH;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}

void xVMathObj::run()
{
    xVObj_Basics::run();
    setStatus(OS_RUNNING);
    _paramMp["equation"]._value.value<xVEvalCondition>().evaluate();
    emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED,0);
    setStatus(OS_VALID);
}

void xVMathObj::save(QDataStream& d,bool _explicit)
{
    xVObj_Basics::save(d,_explicit);
}

void xVMathObj::reset()
{

}

void xVMathObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-10,0);
    path.lineTo(110,0);
    path.lineTo(120,12.5);
    path.arcTo(QRectF(100,12.5,40,25),90,90);

    //path.lineTo(110,25);
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
