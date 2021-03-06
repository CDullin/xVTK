#include "xVConnector.h"
#include <QRectF>
#include "xVObjects.h"

xConnector::xConnector(xVObj_Basics* o):QObject(){
    _type=xCT_INVALID;
    pBaseObj=o;
    _id="CON"+QDateTime::currentDateTime().toString("yyMMddhhmmsszzz")+QString("_%1").arg(::_objIDCount++);
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void*)),o,SIGNAL(KSignal(const SIG_TYPE&,void*)));
    updateToolTip();
}

xVObj_Basics* xConnector::baseObj(){return pBaseObj;}
bool xConnector::isEnabled(){return _enabled;}
QList <xVObj_Basics*>* xConnector::connectedObjects(){return &_connectToLst;}
xCONNECTOR_TYPE xConnector::type(){return _type;}
QGraphicsItemGroup *xConnector::item(){return pGrpItem;}
void xConnector::setActivated(bool b){pEllipseItem->setActivated(b);}
bool xConnector::isActivated(){return pEllipseItem->isActivated();}
QPointF xConnector::pos(){
    QPointF p(15,0);
    switch (_type)
    {
    case xCT_INPUT: p=QPointF(-15,0);break;
    case xCT_OUTPUT: p=QPointF(15,0);break;
    case xCT_PARAMETER: p=QPointF(0,0);break;
    default:
        break;
    }
    return pEllipseItem->mapToScene(p);}
QString xConnector::id(){return _id;}
QStringList* xConnector::objIDs(){return &_idLst;}

void xConnector::updateToolTip()
{
    if (_type==xCT_OUTPUT)
    {
        if (pBaseObj && pEllipseItem)
        {
            QString param = pBaseObj->outputParamMap()->keys().join("\n");
            param.isEmpty() ? pEllipseItem->setToolTip("") : pEllipseItem->setToolTip("output:\n"+param);
        }
    }
    else
    {
        if (_type==xCT_INPUT && pBaseObj && !pBaseObj->inputRequirements()->isEmpty())
        {
            QString txt="input:\n";
            for (int i=0;i<pBaseObj->inputRequirements()->count();++i)
            {
                QString line = QString("[%1] ").arg(i)+pBaseObj->inputRequirements()->at(i).join(" + ")+"\n";
                txt+=line;
            }
            txt.remove(-1,1);
            if (pEllipseItem) pEllipseItem->setToolTip(txt);
        }
        else
        {
            if (pEllipseItem) pEllipseItem->setToolTip("");
        }
    }
}

void xConnector::generateShape()
{
    QPen p2(Qt::black);
    p2.setWidth(2);
    if (!pGrpItem)
    {
        pGrpItem = new QGraphicsItemGroup();
        pLineItem = new QGraphicsLineItem(-10,0,0,0);
        pEllipseItem = new xEllipseItem(QRectF(-20,-5,10,10));
        pEnabledItem = new QGraphicsPathItem();
        QPainterPath path;
        path.moveTo(-3,-3);
        path.lineTo(3,3);
        path.moveTo(3,-3);
        path.lineTo(-3,3);
        pEnabledItem->setPath(path);
        pEnabledItem->setPen(QPen(Qt::red,2));
        pLineItem->setPen(p2);
        pEllipseItem->setPen(p2);
        pGrpItem->addToGroup(pLineItem);
        pGrpItem->addToGroup(pEllipseItem);
        pGrpItem->addToGroup(pEnabledItem);
        pGrpItem->setAcceptHoverEvents(true);
        pGrpItem->setAcceptedMouseButtons(Qt::AllButtons);
        pGrpItem->setHandlesChildEvents(false);
        connect(pEllipseItem,SIGNAL(activated()),this,SLOT(connectorActivated()));
        connect(pEllipseItem,SIGNAL(hoverEnter()),this,SLOT(updateToolTip()));
    }
    switch (_type)
    {
    case xCT_INPUT:
        pLineItem->setLine(-10,0,0,0);
        pEllipseItem->setRect(QRectF(-20,-5,10,10));
        pEllipseItem->setStartAngle(-135*16);
        pEllipseItem->setSpanAngle(270*16);
        pEnabledItem->setPos(-5,0);
        break;
    case xCT_PARAMETER:
        pLineItem->setLine(0,10,0,5);
        pEnabledItem->setPos(0,7);
        pEllipseItem->setRect(QRectF(-5,-5,10,10));
        pEllipseItem->setStartAngle(180*16);
        pEllipseItem->setSpanAngle(180*16);
        break;
    case xCT_OUTPUT:
        pLineItem->setLine(0,0,10,0);
        pEnabledItem->setPos(5,0);
        pEllipseItem->setRect(QRectF(10,-5,10,10));
        break;
    }
    pEnabledItem->setVisible(!_enabled);
}

void xConnector::setEnabled(bool b)
{
    _enabled = b;
    if (pEnabledItem)
        pEnabledItem->setVisible(!_enabled);
}

void xConnector::setToInput()
{
    _type = xCT_INPUT;
    generateShape();
    updateToolTip();
}

void xConnector::setToParamInput()
{
    _type = xCT_PARAMETER;
    generateShape();
    updateToolTip();
}

void xConnector::connectorActivated()
{
    emit activated(this,_type);
}

void xConnector::setToOutput()
{
    _type = xCT_OUTPUT;
    generateShape();
    updateToolTip();
}

void xConnector::addConObject(xVObj_Basics* pVObj){
    _connectToLst.append(pVObj);
    emit KSignal(ST_OBJ_ADDED_TO_CONNECTOR,this);
    updateToolTip();
}
void xConnector::removeConObj(xVObj_Basics* pVObj){
    _connectToLst.removeAll(pVObj);
    emit KSignal(ST_OBJ_REMOVED_FROM_CONNECTOR,this);
    updateToolTip();
}
bool xConnector::connectedTo(xVObj_Basics* pVObj){return _connectToLst.contains(pVObj);}

xConnector::xConnector(xVObj_Basics* pVobj,QDataStream &d):QObject()
{
    pBaseObj=pVobj;
    quint16 v;
    d >> _id;
    d >> v;_type=(xCONNECTOR_TYPE)v;
    d >> _enabled;
    d >> v;
    for (int i=0;i<v;++i)
    {
        QString id; d >> id;
        _idLst.append(id);
    }
    generateShape();
    updateToolTip();
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void* )),pVobj,SIGNAL(KSignal(const SIG_TYPE&,void* )));
}

void xConnector::save(QDataStream &d)
{
    d << _id << (quint16)_type << _enabled;
    d << (quint16)_connectToLst.count();
    for (int i=0;i<_connectToLst.count();++i)
    {
        d << _connectToLst[i]->id();
    }
}
