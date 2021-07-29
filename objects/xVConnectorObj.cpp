#include "xVConnectorObj.h"
#include <QGraphicsDropShadowEffect>
#include <math.h>
#include <stdlib.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDateTime>
#include <QCursor>
#include <math.h>
#include <QPen>

using namespace std;

void xConnectorObj::generatePattern()
{
    _patternTimer.setInterval(250);
    _patternTimer.setSingleShot(false);
    connect(&_patternTimer,SIGNAL(timeout()),this,SLOT(patternTimeOut()));
    _patternTimer.start();
}

void xConnectorObj::patternTimeOut()
{
    _patternId = (_patternId-1) % 8;
    _linePen.setDashOffset(_patternId);
    if (pPathItem) pPathItem->setPen(_linePen);
}

xConnectorObj::xConnectorObj(QDataStream &d):xVAbstractBaseObj(d)
{
    _type = xVOT_CONNECTOR;
    d >> _id;
    d >> _paramConnection;
    // connections are done by main dlg
    quint16 c;d >> c;
    for (int i=0;i<c;++i)
        _sectionLst.append(SECTION(d));
    _dontResetControlPoints = true;
}

xConnectorObj::xConnectorObj(xConnector* pIn,xConnector* pOut,bool _param):xVAbstractBaseObj()
{
    _type = xVOT_CONNECTOR;
    _paramConnection = _param;
    pInObj=pIn;
    pOutObj=pOut;
    _id="COB"+QDateTime::currentDateTime().toString("yyMMddhhmmsszzz")+QString("_%1").arg(::_objIDCount++);
    _paramMp["show arrows"]._id = 1;
    _paramMp["show arrows"]._value = false;

    generateShape();
}

QGraphicsPathItem* xConnectorObj::generateArrow()
{
    QGraphicsPathItem* pArrowItem = new QGraphicsPathItem();
    QPainterPath path;
    path.moveTo(-2.5,0);
    path.lineTo(-7.5,-7.5);
    path.lineTo(10,0);
    path.lineTo(-7.5,7.5);
    path.lineTo(-2.5,0);
    pArrowItem->setPath(path);
    QPen p(Qt::black);
    QBrush br(Qt::black);
    pArrowItem->setPen(p);
    pArrowItem->setBrush(br);
    pArrowItem->setScale(0.75);
    return pArrowItem;
}

void xConnectorObj::updatePath()
{
    QPointF oPnt = pOutObj->pos();
    QPointF iPnt; _paramConnection ? iPnt = pInObj->pos() : iPnt = pInObj->pos();
    if (pInObj->type()==xCT_INPUT) iPnt-=QPointF(15,0);
    else iPnt-=QPointF(0,18);

    for (int i=0;i<_arrowLst.count();++i)
        pGrpItem->scene()->removeItem(_arrowLst[i]);
    _arrowLst.clear();

    QPainterPath path2;
    path2.moveTo(oPnt);
    path2.addEllipse(QRectF(oPnt.x()-2,oPnt.y()-2,4,4));
    QPainterPath path;
    path.moveTo(oPnt);
    for (int i=0;i<_sectionLst.count()-1;++i)
    {
        _sectionLst[i]._horizontal ? path.lineTo(_sectionLst[i+1]._pos,_sectionLst[i]._pos) : path.lineTo(_sectionLst[i]._pos,_sectionLst[i+1]._pos);
        xVHistoNodeItem *pNode=nullptr;
        for (int j=0;j<_nodeLst.count();++j)
            if (_nodeLst[j]->nr()==i)
                pNode=_nodeLst[j];
        if (i>0)
        {
            if (!pNode)
                _sectionLst[i]._horizontal ? addNode(QPointF((_sectionLst[i+1]._pos+_sectionLst[i-1]._pos)/2.0,_sectionLst[i]._pos),i,xVHistoNodeItem::HNIM_VERTICAL) :
                    addNode(QPointF(_sectionLst[i]._pos,(_sectionLst[i+1]._pos+_sectionLst[i-1]._pos)/2.0),i,xVHistoNodeItem::HNIM_HORIZONTAL);
            else
            {
                _sectionLst[i]._horizontal ? pNode->setPos((_sectionLst[i+1]._pos+_sectionLst[i-1]._pos)/2.0,_sectionLst[i]._pos) :
                    pNode->setPos(_sectionLst[i]._pos,(_sectionLst[i+1]._pos+_sectionLst[i-1]._pos)/2.0);
            }
        }
    }
    path.lineTo(iPnt);
    pPathItem->setPath(path);
    pPathStartItem->setPath(path2);

    // we put at path length
    float len=path.length();
    float distance = 50;
    for (long j=0;j<len/distance-1;++j)
    {
        float rel = (((double)j+1)*distance)/len;
        if (rel<0 || (rel+0.005)>1) continue;
        QPointF pos = path.pointAtPercent(rel);
        QPointF posAhead = path.pointAtPercent(rel+0.005);
        float _angle = 0;
        if (fabs(posAhead.x()-pos.x())<0.00001) // vertical
        {
            if (posAhead.y()>pos.y()) // up
                _angle=90;
            else
                _angle=-90;
        }
        else
        {
            if (posAhead.x()>pos.x())
                _angle = 0;
            else
                _angle = 180;
        }
        QGraphicsPathItem *pArrow=generateArrow();
        pArrow->setPos(pos);
        pArrow->setRotation(_angle);
        pGrpItem->addToGroup(pArrow);
        _arrowLst.append(pArrow);
    }

    // add end symbol
    QPainterPath endPath;
    if (pInObj->type()==xCT_INPUT)
    {
        endPath.lineTo(5,0);
        endPath.lineTo(5,-5);
        endPath.lineTo(15,0);
        endPath.lineTo(5,5);
        endPath.lineTo(5,0);
    }
    else {
        endPath.lineTo(0,10);
        pEllipseItem->setRect(-5,10,10,10);
        pEllipseItem->setStartAngle(0*16);
        pEllipseItem->setSpanAngle(180*16);
        pEllipseItem->setPos(0,0);
    }
    pEndItem->setPath(endPath);
    pEndGrp->setPos(iPnt);
    pPathItem->setPath(path);
    pPathStartItem->setPath(path2);

    for (int i=0;i<_nodeLst.count();++i) _nodeLst[i]->setVisible(_selected);
    for (int i=0;i<_arrowLst.count();++i) _arrowLst[i]->setVisible(!_selected && _paramMp["show arrows"]._value.toBool());

    _dontResetControlPoints = false;        
}

void xConnectorObj::addNodeAtCursorPos()
{
    QPoint globalPos = QCursor::pos();
    QPoint viewPos = pGrpItem->scene()->views().at(0)->mapFromGlobal(globalPos);
    QPointF scenePos = pGrpItem->scene()->views().at(0)->mapToScene(viewPos);
    QPointF itemPos=pGrpItem->mapFromScene(scenePos);

    // find insert position
    int _insertPos = _sectionLst.count()-1;
    if (_sectionLst.at(_insertPos)._horizontal)
    {
        _sectionLst.insert(_insertPos,SECTION(itemPos.y(),true));
        addNode(itemPos,_insertPos,xVHistoNodeItem::HNIM_VERTICAL);
        _sectionLst.insert(_insertPos+1,SECTION(itemPos.x(),false));
        addNode(itemPos,_insertPos+1,xVHistoNodeItem::HNIM_HORIZONTAL);
    }
    else
    {
        _sectionLst.insert(_insertPos,SECTION(itemPos.x(),false));
        addNode(itemPos,_insertPos,xVHistoNodeItem::HNIM_HORIZONTAL);
        _sectionLst.insert(_insertPos+1,SECTION(itemPos.y(),true));
        addNode(itemPos,_insertPos+1,xVHistoNodeItem::HNIM_VERTICAL);
    }

    updatePath();
}

void xConnectorObj::addNode(QPointF p,int i,xVHistoNodeItem::HNI_MODE m)
{
    xVHistoNodeItem* pNode=new xVHistoNodeItem(i,m,this);
    pNode->setPos(p);
    pNode->setColor(Qt::black);
    pNode->setLineColor(Qt::black);
    pNode->setGraphicsEffect(0);
    connect(pNode,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(nodeMoved(QPointF,const int&,xVHistoNodeItem*)));
    pGrpItem->addToGroup(pNode);
    _nodeLst.append(pNode);
}

QPointF snapToGrid(QGraphicsItem* item,QPointF p)
{
    QPointF scenePos = item->mapToScene(p);
    /*
    if (::_settings["snap to grid"]._value.toBool())
    {
        scenePos.setX(nearbyint(scenePos.x()/::_settings["grid resolution"]._value.toDouble())*::_settings["grid resolution"]._value.toDouble());
        scenePos.setY(nearbyint(scenePos.y()/::_settings["grid resolution"]._value.toDouble())*::_settings["grid resolution"]._value.toDouble());
    }
    */
    return item->mapFromScene(scenePos);
}

void xConnectorObj::nodeMoved(QPointF p,const int&,xVHistoNodeItem* pNode)
{
    int nr = pNode->nr();
    pNode->setPos(snapToGrid(pNode,pNode->pos()));
    if (nr>=0 && nr<_sectionLst.count())
    {
        _sectionLst[nr]._horizontal ? _sectionLst[nr]._pos=pNode->pos().y() : _sectionLst[nr]._pos=pNode->pos().x();
        updatePath();
    }
}

void xConnectorObj::createPath()
{
    if (!_dontResetControlPoints)
    {
        //alternating vertical and horizonal sections
        QPointF oPnt = pOutObj->pos();
        QPointF iPnt; _paramConnection ? iPnt = pInObj->pos() : iPnt = pInObj->pos();
        if (pInObj->type()==xCT_INPUT) iPnt-=QPointF(15,0);
        else iPnt-=QPointF(0,18);
        float dx = iPnt.x()-oPnt.x();
        float dy = iPnt.y()-oPnt.y();

        _sectionLst.clear();
        for (int j=0;j<_nodeLst.count();++j)
        {
            pGrpItem->removeFromGroup(_nodeLst[j]);
            pGrpItem->scene()->removeItem(_nodeLst[j]);
            delete _nodeLst[j];
        }
        _nodeLst.clear();

        if (fabs(dx)>fabs(dy) && dx>0) // we start horizontal
        {
            _sectionLst.append(SECTION(oPnt.y(),true));
            _sectionLst.append(SECTION(oPnt.x()+dx/2,false));
            addNode(QPointF(oPnt.x()+dx/2,oPnt.y()+dy/2),1,xVHistoNodeItem::HNIM_HORIZONTAL);
            _sectionLst.append(SECTION(iPnt.y(),true));
        }
        else
        {
            _sectionLst.append(SECTION(oPnt.x(),false));
            _sectionLst.append(SECTION(oPnt.y()+dy/2,true));
            addNode(QPointF(oPnt.x()+dx/2,oPnt.y()+dy/2),1,xVHistoNodeItem::HNIM_VERTICAL);
            _sectionLst.append(SECTION(iPnt.x(),false));
        }
    }
    updatePath();
}

void xConnectorObj::generateShape()
{
    if (pInObj && pOutObj)
    {
        QPen p(Qt::black);
        p.setWidth(2);

        _linePen.setColor(Qt::black);
        _linePen.setWidth(3);
        _linePen.setStyle(Qt::DashDotLine);

        pGrpItem = new xGroupItem();
        pEndGrp = new QGraphicsItemGroup();
        pEndItem = new QGraphicsPathItem();
        pEllipseItem = new QGraphicsEllipseItem;

        pEndGrp->addToGroup(pEllipseItem);
        pEndGrp->addToGroup(pEndItem);
        pGrpItem->addToGroup(pEndGrp);
        pPathItem = new QGraphicsPathItem();
        pGrpItem->addToGroup(pPathItem);
        pPathItem->setPen(_linePen);
        pPathStartItem = new QGraphicsPathItem();
        pGrpItem->addToGroup(pPathStartItem);
        pPathStartItem->setPen(p);
        pEllipseItem->setPen(p);
        pEndItem->setPen(QPen(Qt::black,3));

        createPath();

        pEffect=new QGraphicsDropShadowEffect();
        pEffect->setBlurRadius(5);
        pEffect->setOffset(QPointF(3,3));
        pGrpItem->setGraphicsEffect(pEffect);

        pGrpItem->setAcceptHoverEvents(true);
        pGrpItem->setAcceptedMouseButtons(Qt::AllButtons);
        pGrpItem->setHandlesChildEvents(false);
        connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));

        for (int i=0;i<_arrowLst.count();++i)
            _arrowLst[i]->setVisible(_paramMp["show arrows"]._value.toBool());

        connect(pInObj->baseObj(),SIGNAL(moved()),this,SLOT(update()));
        connect(pOutObj->baseObj(),SIGNAL(moved()),this,SLOT(update()));

        pSelEffect = new QGraphicsDropShadowEffect();
        pSelEffect->setColor(Qt::red);
        pSelEffect->setBlurRadius(30);
        pSelEffect->setOffset(QPointF(0,0));
        paAnimation2 = new QPropertyAnimation(pSelEffect,"color",this);
        paAnimation2->setStartValue(QColor(Qt::red));
        paAnimation2->setKeyValueAt(0.5,QColor(Qt::yellow));
        paAnimation2->setEndValue(QColor(Qt::red));
        paAnimation2->setDuration(800);
        pSelectionTimer = new QTimer();
        pSelectionTimer->setInterval(1000);
        pSelectionTimer->setSingleShot(false);
        connect(pSelectionTimer,SIGNAL(timeout()),paAnimation2,SLOT(start()));

        generatePattern();
    }
}

void xConnectorObj::setParamSelected(bool b)
{
    _selected = b;
    if (pGrpItem!=nullptr)
    {
        QPen p;
        if (_selected)
        {
            disconnect(pSelectionTimer,0,0,0);
            pSelEffect = new QGraphicsDropShadowEffect();
            pSelEffect->setColor(Qt::red);
            pSelEffect->setBlurRadius(30);
            pSelEffect->setOffset(QPointF(0,0));
            paAnimation2 = new QPropertyAnimation(pSelEffect,"color",this);
            paAnimation2->setStartValue(QColor(Qt::red));
            paAnimation2->setKeyValueAt(0.5,QColor(Qt::yellow));
            paAnimation2->setEndValue(QColor(Qt::red));
            paAnimation2->setDuration(800);
            if (pGrpItem) pGrpItem->setGraphicsEffect(pSelEffect);
            pSelEffect->setEnabled(true);
            pSelectionTimer->start();
            connect(pSelectionTimer,SIGNAL(timeout()),paAnimation2,SLOT(start()));

            for (int i=0;i<_nodeLst.count();++i) _nodeLst[i]->setVisible(true);
            for (int i=0;i<_arrowLst.count();++i) _arrowLst[i]->setVisible(false);
        }
        else
        {
            pSelectionTimer->stop();
            pSelEffect->setEnabled(false);
            pEffect=new QGraphicsDropShadowEffect();
            pEffect->setBlurRadius(5);
            pEffect->setOffset(QPointF(3,3));
            if (pGrpItem) pGrpItem->setGraphicsEffect(pEffect);

            for (int i=0;i<_nodeLst.count();++i) _nodeLst[i]->setVisible(false);
            for (int i=0;i<_arrowLst.count();++i) _arrowLst[i]->setVisible(_paramMp["show arrows"]._value.toBool());
        }
    }
}


xConnectorObj::~xConnectorObj()
{
    if (pGrpItem)
    {
        _patternTimer.stop();
        if (pSelectionTimer) pSelectionTimer->stop();
        pGrpItem->scene()->removeItem(pGrpItem);
    }
}

void xConnectorObj::update()
{
    QPointF oPnt = pOutObj->pos();
    QPointF iPnt; _paramConnection ? iPnt = pInObj->pos() : iPnt = pInObj->pos();
    if (pInObj->type()==xCT_INPUT) iPnt-=QPointF(15,0);
    else iPnt-=QPointF(0,18);

    if (iPnt!=oldIPnt || oPnt!=oldOPnt) createPath();
    else updatePath();

    oldIPnt = iPnt;
    oldOPnt = oPnt;

    for (int i=0;i<_arrowLst.count();++i)
        _arrowLst[i]->setVisible(_paramMp["show arrows"]._value.toBool());
}

void xConnectorObj::save(QDataStream& d, bool _explicit)
{
    xVAbstractBaseObj::save(d,_explicit);
    d << _id << _paramConnection;
    d << (quint16) _sectionLst.count();
    for (int i=0;i<_sectionLst.count();++i)
        _sectionLst[i].save(d);
    d << pInObj->id() << pOutObj->id();
}
