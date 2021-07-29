#include "xVWaitObj.h"
#include <QApplication>
#include <QThread>

xVWaitObj::xVWaitObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_WAIT;
    _description="Used to specifically delay the progress in the current fiber";
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;

    _paramMp["total wait time [ms]"]._value = QVariant::fromValue<xLimitedDouble>(xLimitedDouble(5000,0,1E12,10));
    _paramMp["total wait time [ms]"]._id = 1;

    _paramMp["ticks"]._value = QVariant::fromValue<xLimitedInt>(xLimitedInt(30,1,100));
    _paramMp["ticks"]._id = 2;

    _maxOutput=1000;
    _maxInput=1000;

    generateShape();

    xConnector* pInputCon = new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    //pInputCon->item()->setRotation(45);
    pInputCon->item()->moveBy(0,12.5);

    pGrpItem->addToGroup(pInputCon->item());
    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    //pOutputCon->item()->setRotation(-45);
    pOutputCon->item()->moveBy(40,12.5);
    pGrpItem->addToGroup(pOutputCon->item());
    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(20,-10);
    pGrpItem->addToGroup(pParamInputCon->item());

    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    tim.setSingleShot(false);
    connect(&tim,SIGNAL(timeout()),this,SLOT(updateTimeDisplay()));
    tim.setInterval(_paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value / (double)_paramMp["ticks"]._value.value<xLimitedInt>()._value);
}

xVWaitObj::xVWaitObj(QDataStream& d):xVObj_Basics(d)
{
    _type = xVOT_WAIT;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
    tim.setSingleShot(false);
    connect(&tim,SIGNAL(timeout()),this,SLOT(updateTimeDisplay()));
    tim.setInterval(_paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value / (double)_paramMp["ticks"]._value.value<xLimitedInt>()._value);
}

void xVWaitObj::run()
{
    xVObj_Basics::run();
    setStatus(OS_RUNNING);

    _totalTimeReached=false;
    _time = 0;
    tim.setInterval(_paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value / (double)_paramMp["ticks"]._value.value<xLimitedInt>()._value);
    tim.start();
    // calculate equation(s)
    while (!_totalTimeReached)
    {
        qApp->processEvents();
    }

    setStatus(OS_VALID);
}

void xVWaitObj::paramModified(const QString& txt)
{
    xVObj_Basics::paramModified(txt);
    tim.setInterval(_paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value / (double)_paramMp["ticks"]._value.value<xLimitedInt>()._value);
}

void xVWaitObj::reset()
{
    setStatus(OS_UPDATE_NEEDED);
    _totalTimeReached=false;
    _time = 0;
    tim.setInterval(_paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value / (double)_paramMp["ticks"]._value.value<xLimitedInt>()._value);
    updateTimeDisplay();
}

void xVWaitObj::updateTimeDisplay()
{
    _time+=tim.interval();
    _totalTimeReached = _time>=_paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value;
    if (_totalTimeReached) {tim.stop();return;}
    float _angle = _time / _paramMp["total wait time [ms]"]._value.value<xLimitedDouble>()._value * 360.0;
    pPointer->setRotation(_angle);
    QPainterPath pp;
    pp.lineTo(0,-18);
    pp.arcTo(QRectF(-18,-18,36,36),90.0,-_angle);
    pp.lineTo(0,0);
    pBack->setPath(pp);
}

void xVWaitObj::generateShape()
{
    QPainterPath path;
    path.moveTo(0,0);
    path.addEllipse(QRectF(0,0,40,40));
    path.moveTo(20,10);

    for (int i=0;i<4;++i)
    {
        path.arcTo(QRectF(10,10,20,20),(90-5+i*90) % 360,10);
        path.arcTo(QRectF(5,5,30,30),(90+5+i*90) % 360,80);
    }
    path.arcTo(QRectF(10,10,20,20),(90-5+4*90) % 360,10);

    QPainterPath pp;
    pp.addEllipse(QRectF(-3,-3,6,6));
    pp.moveTo(-3,0);
    pp.lineTo(-3,-7);
    pp.lineTo(0,-17);
    pp.lineTo(3,-7);
    pp.lineTo(3,6);
    pp.lineTo(-3,6);
    pp.lineTo(-3,0);

    //path.addEllipse(QRectF(5,5,30,30));

    QPen pen(Qt::black);
    QRadialGradient rg(-5,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(250,250,0));
    rg.setColorAt(1,QColor(150,150,0));

    QBrush brush(rg);
    pShapeItem = new xGraphicsPathItem(path);
    connect(pShapeItem,SIGNAL(hoverEnter()),this,SLOT(updateDescToolTip()));
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pPointer = new QGraphicsPathItem(pp);
    pPointer->setPen(QPen(Qt::black));
    pPointer->setBrush(QBrush(Qt::red));
    pPointer->setPos(20,20);

    pBack = new QGraphicsPathItem();
    pBack->setPen(QPen(Qt::green));
    pBack->setBrush(QBrush(Qt::green));
    pBack->setPos(20,20);
    pBack->setZValue(-10);

    pTxtItem = new xScrollTxtItem(80,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);
    pTxtItem->hide();

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->addToGroup(pPointer);
    pGrpItem->addToGroup(pBack);
    pStatusItem->setPos(35,-5);
    pGrpItem->addToGroup(pStatusItem);
    pGrpItem->setBoundingRectSize(QRectF(-22,-15,85,60));
/*
    pBoundingRectItem = new QGraphicsRectItem(pGrpItem->boundingRect());
    pBoundingRectItem->setPen(QPen(Qt::white,2));
    pBoundingRectItem->setBrush(QBrush(Qt::NoBrush));
    pGrpItem->addToGroup(pBoundingRectItem);
*/
    pGrpItem->setFlags(QGraphicsItem::ItemIsMovable);
    pEffect=new QGraphicsDropShadowEffect();
    pEffect->setBlurRadius(5);
    pEffect->setOffset(QPointF(3,3));
    pGrpItem->setGraphicsEffect(pEffect);

    pGrpItem->setAcceptHoverEvents(true);
    pGrpItem->setAcceptedMouseButtons(Qt::AllButtons);
    pGrpItem->setHandlesChildEvents(false);
}
