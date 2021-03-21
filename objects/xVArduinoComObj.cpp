#include "xVArduinoComObj.h"

xVArduinoComObj::xVArduinoComObj(const QString& txt):xVObj_Basics()
{
    _type=xVOT_ARDUINO_COM;
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
    pOutputCon->item()->moveBy(95,12.5);
    pGrpItem->addToGroup(pOutputCon->item());
    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(52-7,-10);
    pGrpItem->addToGroup(pParamInputCon->item());

    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}
xVArduinoComObj::xVArduinoComObj(QDataStream& d):xVObj_Basics(d)
{
    _type=xVOT_ARDUINO_COM;
    _maxInput=1000;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}
void xVArduinoComObj::run()
{
    xVObj_Basics::run();
    setStatus(OS_RUNNING);
    // ... com
    setStatus(OS_VALID);
}
void xVArduinoComObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-15,0);
    path.lineTo(60-25,0);
    path.arcTo(QRectF(80-25,-6.5,15,12.5),180,180);
    path.lineTo(105-25,0);
    path.lineTo(110-25,6.5);
    path.arcTo(QRectF(105-25,6.25,15,12.5),90,-180);
    path.lineTo(110-25,25-6.5);
    path.lineTo(105-25,25);

    path.lineTo(-15,25);
    path.lineTo(-10,25-6.5);
    path.arcTo(QRectF(-15,6.25,15,12.5),270,180);
    path.lineTo(-10,6.5);
    path.lineTo(-15,0);

    QPen pen(Qt::black);
    QRadialGradient rg(-15,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(0,255,0));
    rg.setColorAt(0.4,QColor(255,255,255));
    rg.setColorAt(0.6,QColor(255,255,255));
    rg.setColorAt(1,QColor(255,0,0));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(105,_paramMp["name"]._value.toString());
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
