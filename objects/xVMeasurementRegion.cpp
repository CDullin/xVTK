#include "xVMeasurementRegion.h"
#include <QVector3D>
#include "xVTypes.h"

xVMeasurementRegion::xVMeasurementRegion(const QString& txt):xVObj_Basics()
{
    _type = xVOT_MEASUREMENT_REGION;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    addToList(_optionLsts["region type"],QStringList() << "box" << "cylindric" << "ellipsoid" << "3D polyline" << "mesh");
    // box = rectangle and line in 2D
    // cylindric = circle in 2D
    // ellipsoid = elliptic in 2D
    // 3D polyline = 2D polyline
    // mesh = needs to be imported
    _paramMp["region type"]._id = 1;
    _paramMp["region type"]._value = "box";
    _paramMp["box dimension [px]"]._id = 5;
    _paramMp["box dimension [px]"]._value = QVariant::fromValue(QVector3D(10,10,10));
    _paramMp["box dimension [px]"]._subGrp = "box";
    _paramMp["cylinder radius [px]"]._id = 10;
    _paramMp["cylinder radius [px]"]._value = QVariant::fromValue(xLimitedDouble(10,0,1000,10));
    _paramMp["cylinder radius [px]"]._subGrp = "cylinder";
    _paramMp["cylinder height [px]"]._id = 11;
    _paramMp["cylinder height [px]"]._value = QVariant::fromValue(xLimitedDouble(50,0,1000,10));
    _paramMp["cylinder height [px]"]._subGrp = "cylinder";
    _paramMp["ellipsoid dimension [px]"]._id = 15;
    _paramMp["ellipsoid dimension [px]"]._value = QVariant::fromValue(QVector3D(10,10,10));
    _paramMp["ellipsoid dimension [px]"]._subGrp = "ellipsoid";

    // position definition in space
    _paramMp["center [px]"]._id = 6;
    _paramMp["center [px]"]._value = QVariant::fromValue(QVector3D(10,10,10));


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

xVMeasurementRegion::xVMeasurementRegion(QDataStream &d):xVObj_Basics(d)
{
    _type = xVOT_MEASUREMENT_REGION;

    generateShape();
}

void xVMeasurementRegion::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        setStatus(OS_VALID);
    }
}

void xVMeasurementRegion::reset()
{
    setStatus(OS_UPDATE_NEEDED);
}

void xVMeasurementRegion::save(QDataStream &d,bool _explicit)
{
    xVObj_Basics::save(d,_explicit);
}

void xVMeasurementRegion::generateShape()
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
    rg.setColorAt(0,QColor(10,100,10));
    rg.setColorAt(1,QColor(20,200,20));

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

void xVMeasurementRegion::paramModified(const QString& txt)
{
    xVObj_Basics::paramModified(txt);
}
