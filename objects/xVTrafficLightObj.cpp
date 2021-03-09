#include "xVTrafficLightObj.h"

xVTrafficLightObj::xVTrafficLightObj(QString txt):xVObj_Basics()
{
    _type = xVOT_TRAFFIC_LIGHT;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["traffic light status"]._id=2;
    _paramMp["traffic light status"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,3));
    generateShape();

    xConnector* pInputCon=new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(-9,12.5);
    pGrpItem->addToGroup(pInputCon->item());
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(2,-7);
    pGrpItem->addToGroup(pParamInputCon->item());
    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVTrafficLightObj::xVTrafficLightObj(QDataStream& d):xVObj_Basics(d)
{
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}
void xVTrafficLightObj::run()
{
    setStatus(OS_RUNNING);
    //*******************
    setStatus(OS_VALID);
}

void xVTrafficLightObj::paramModified(const QString &txt)
{
    xVObj_Basics::paramModified(txt);
    switch (_paramMp["traffic light status"]._value.value<xLimitedInt>()._value)
    {
    case 0 : pTrafficLightPixItem->setPixmap(QPixmap("://images/traffic_light_empty.png"));
        break;
    case 1 : pTrafficLightPixItem->setPixmap(QPixmap("://images/traffic_light_green.png"));
        break;
    case 2 : pTrafficLightPixItem->setPixmap(QPixmap("://images/traffic_light_yellow.png"));
        break;
    case 3 : pTrafficLightPixItem->setPixmap(QPixmap("://images/traffic_light_red.png"));
        break;
    }
}

void xVTrafficLightObj::save(QDataStream& d, bool _explicit)
{
    xVObj_Basics::save(d,_explicit);
}

void xVTrafficLightObj::generateShape()
{
    QPainterPath path;
    path.addRoundedRect(QRectF(0,0,120,25),5,5);

    QPen pen(Qt::black);
    QRadialGradient rg(10,10, 100, 0, 1, 5);
    rg.setColorAt(0,QColor(255,0,255));
    rg.setColorAt(1,QColor(0,255,0));

    QBrush brush(rg);
    pTrafficLightPixItem = new QGraphicsPixmapItem();
    pTrafficLightPixItem->setPixmap(QPixmap("://images/traffic_light_empty.png"));
    pTrafficLightPixItem->moveBy(-9,0);
    pTxtItem = new xScrollTxtItem(110,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(15,5);
    pTxtItem->hide();

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pTrafficLightPixItem);
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
