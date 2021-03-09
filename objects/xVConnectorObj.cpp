#include "xVConnectorObj.h"
#include <QGraphicsDropShadowEffect>
#include <math.h>
#include <stdlib.h>
#include <QGraphicsScene>
#include <QDateTime>

using namespace std;

xConnectorObj::xConnectorObj(QDataStream &d):xAbstractBasisObj(d)
{
    d >> _id;
    d >> _paramConnection;
    // connections are done by main dlg
}

xConnectorObj::xConnectorObj(xConnector* pIn,xConnector* pOut,bool _param):xAbstractBasisObj()
{
    _paramConnection = _param;
    pInObj=pIn;
    pOutObj=pOut;
    _id="COB"+QDateTime::currentDateTime().toString("yyMMddhhmmsszzz")+QString("_%1").arg(::_objIDCount++);
    _paramMp["show arrows"]._id = 1;
    _paramMp["show arrows"]._value = true;

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
    for (int i=0;i<_arrowLst.count();++i)
        pGrpItem->scene()->removeItem(_arrowLst[i]);
    _arrowLst.clear();

    QList <QPointF> _pntLst;
    QPointF oPnt = pOutObj->pos();
    QPointF iPnt; _paramConnection ? iPnt = pInObj->pos() : iPnt = pInObj->pos();

    if (pInObj->type()==xCT_INPUT) iPnt-=QPointF(15,0);
    else iPnt-=QPointF(0,18);

    _pntLst.append(oPnt);
    for (int i=0;i<_nodeLst.count();++i)
        _pntLst.append(_nodeLst[i]->pos());
    _pntLst.append(iPnt);

    QPainterPath path;
    path.moveTo(oPnt);
    path.addEllipse(QRectF(oPnt.x()-2,oPnt.y()-2,4,4));
    path.moveTo(oPnt);

    float dxx = iPnt.x()-oPnt.x();
    float dyy = iPnt.y()-oPnt.y();

    bool _horizontal = fabs(dxx)>fabs(dyy);
    if (dxx<0) _horizontal=false;
    //_horizontal=!_horizontal;

    for (int j=0;j<_pntLst.count()-1;++j)
    {
        float dx = _pntLst[j+1].x()-_pntLst[j].x();
        float dy = _pntLst[j+1].y()-_pntLst[j].y();

        if (_horizontal)
        {
            path.lineTo(_pntLst[j].x()+dx/2,_pntLst[j].y());
            path.lineTo(_pntLst[j].x()+dx/2,_pntLst[j+1].y());
            if (fabs(dx)>20)
            {
                QGraphicsPathItem *pArrow=generateArrow();
                pArrow->setPos(_pntLst[j].x()+dx/4,_pntLst[j].y());
                if (dx<0) pArrow->setRotation(180);
                pGrpItem->addToGroup(pArrow);
                _arrowLst.append(pArrow);

                if (fabs(dy)>20)
                {
                    pArrow=generateArrow();
                    pArrow->setPos(_pntLst[j].x()+dx/2,_pntLst[j].y()+dy/2);
                    pArrow->setRotation(90);
                    if (dy<0) pArrow->setRotation(-90);
                    pGrpItem->addToGroup(pArrow);
                    _arrowLst.append(pArrow);
                }

                pArrow=generateArrow();
                pArrow->setPos(_pntLst[j].x()+3*dx/4,_pntLst[j+1].y());
                if (dx<0) pArrow->setRotation(180);
                pGrpItem->addToGroup(pArrow);
                _arrowLst.append(pArrow);
            }
        }
        else
        {
            path.lineTo(_pntLst[j].x(),_pntLst[j].y()+dy/2);
            path.lineTo(_pntLst[j+1].x(),_pntLst[j].y()+dy/2);
            if (fabs(dy)>20)
            {
                QGraphicsPathItem *pArrow=generateArrow();
                pArrow->setPos(_pntLst[j].x(),_pntLst[j].y()+dy/4);
                pArrow->setRotation(90);
                if (dy<0) pArrow->setRotation(-90);
                pGrpItem->addToGroup(pArrow);
                _arrowLst.append(pArrow);

                if (fabs(dx)>20)
                {
                    pArrow=generateArrow();
                    pArrow->setPos(_pntLst[j].x()+dx/2,_pntLst[j].y()+dy/2);
                    if (dx<0) pArrow->setRotation(180);
                    pGrpItem->addToGroup(pArrow);
                    _arrowLst.append(pArrow);
                }

                pArrow=generateArrow();
                pArrow->setPos(_pntLst[j+1].x(),_pntLst[j].y()+3*dy/4);
                pArrow->setRotation(90);
                if (dy<0) pArrow->setRotation(-90);
                pGrpItem->addToGroup(pArrow);
                _arrowLst.append(pArrow);
            }
        }
        path.lineTo(_pntLst[j+1]);
        _horizontal=!_horizontal;
    }
    pPathItem->setPath(path);
}

void xConnectorObj::createPath()
{
    for (int i=0;i<_arrowLst.count();++i)
    {
        pGrpItem->scene()->removeItem(_arrowLst[i]);
    }
    _arrowLst.clear();
    for (int i=0;i<_nodeLst.count();++i)
    {
        pGrpItem->scene()->removeItem(_nodeLst[i]);
    }
    _nodeLst.clear();

    QPointF oPnt = pOutObj->pos();
    QPointF iPnt; _paramConnection ? iPnt = pInObj->pos() : iPnt = pInObj->pos();

    if (pInObj->type()==xCT_INPUT) iPnt-=QPointF(15,0);
    else iPnt-=QPointF(0,18);

    float dx = iPnt.x()-oPnt.x();
    float dy = iPnt.y()-oPnt.y();

    QPainterPath path;
    path.moveTo(oPnt);
    path.addEllipse(QRectF(oPnt.x()-2,oPnt.y()-2,4,4));
    path.moveTo(oPnt);

    if ((fabs(dx)>fabs(dy) && dx>0) || (dy<0 && pInObj->type()==xCT_PARAMETER))
    {
        path.lineTo(oPnt.x()+dx/2,oPnt.y());
        path.lineTo(oPnt.x()+dx/2,iPnt.y());
        xVHistoNodeItem *p0=new xVHistoNodeItem(0,xVHistoNodeItem::HNIM_VERTICAL|xVHistoNodeItem::HNIM_HORIZONTAL);
        p0->setPos(oPnt.x()+dx/2,oPnt.y()+dy/2);p0->setGraphicsEffect(0);p0->setLineColor(Qt::black);p0->setColor(QColor(0,0,0,0));
        xVHistoNodeItem *p1=new xVHistoNodeItem(0,xVHistoNodeItem::HNIM_VERTICAL|xVHistoNodeItem::HNIM_HORIZONTAL);
        p1->setPos(oPnt.x()+dx/2,iPnt.y());p1->setGraphicsEffect(0);p1->setLineColor(Qt::black);p1->setColor(QColor(0,0,0,0));
        _nodeLst.append(p0);
        //_nodeLst.append(p1);
        connect(p0,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(updatePath()));
        connect(p1,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(updatePath()));
        pGrpItem->addToGroup(p0);
        //pGrpItem->addToGroup(p1);

        if (fabs(dx)>40)
        {
            QGraphicsPathItem *pArrow=generateArrow();
            pArrow->setPos(oPnt.x()+dx/4,oPnt.y());
            if (dx<0) pArrow->setRotation(180);
            pGrpItem->addToGroup(pArrow);
            _arrowLst.append(pArrow);

            if (fabs(dy)>20)
            {
                pArrow=generateArrow();
                pArrow->setPos(oPnt.x()+dx/2,oPnt.y()+dy/2);
                pArrow->setRotation(90);
                if (dy<0) pArrow->setRotation(-90);
                pGrpItem->addToGroup(pArrow);
                _arrowLst.append(pArrow);
            }

            pArrow=generateArrow();
            pArrow->setPos(oPnt.x()+3*dx/4,iPnt.y());
            if (dx<0) pArrow->setRotation(180);
            pGrpItem->addToGroup(pArrow);
            _arrowLst.append(pArrow);
        }
    }
    else
    {
        path.lineTo(oPnt.x(),oPnt.y()+dy/2);
        path.lineTo(iPnt.x(),oPnt.y()+dy/2);

        xVHistoNodeItem *p0=new xVHistoNodeItem(0,xVHistoNodeItem::HNIM_VERTICAL|xVHistoNodeItem::HNIM_HORIZONTAL);
        p0->setPos(oPnt.x()+dx/2,oPnt.y()+dy/2);p0->setGraphicsEffect(0);p0->setLineColor(Qt::black);p0->setColor(QColor(0,0,0,0));
        xVHistoNodeItem *p1=new xVHistoNodeItem(0,xVHistoNodeItem::HNIM_VERTICAL|xVHistoNodeItem::HNIM_HORIZONTAL);
        p1->setPos(iPnt.x(),oPnt.y()+dy/2);p1->setGraphicsEffect(0);p1->setLineColor(Qt::black);p1->setColor(QColor(0,0,0,0));
        _nodeLst.append(p0);
        //_nodeLst.append(p1);
        connect(p0,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(updatePath()));
        connect(p1,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(updatePath()));
        pGrpItem->addToGroup(p0);
        //pGrpItem->addToGroup(p1);

        if (fabs(dy)>40)
        {
            QGraphicsPathItem *pArrow=generateArrow();
            pArrow->setPos(oPnt.x(),oPnt.y()+dy/4);
            pArrow->setRotation(90);
            if (dy<0) pArrow->setRotation(-90);
            pGrpItem->addToGroup(pArrow);
            _arrowLst.append(pArrow);

            if (fabs(dx)>20)
            {
                pArrow=generateArrow();
                pArrow->setPos(oPnt.x()+dx/2,oPnt.y()+dy/2);
                if (dx<0) pArrow->setRotation(180);
                pGrpItem->addToGroup(pArrow);
                _arrowLst.append(pArrow);
            }

            pArrow=generateArrow();
            pArrow->setPos(iPnt.x(),oPnt.y()+3*dy/4);
            pArrow->setRotation(90);
            if (dy<0) pArrow->setRotation(-90);
            pGrpItem->addToGroup(pArrow);
            _arrowLst.append(pArrow);
        }
    }
    path.lineTo(iPnt);
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
    pGrpItem->setBoundingRectSize(QRectF(oPnt,iPnt));

    for (int i=0;i<_nodeLst.count();++i)
        _nodeLst[i]->setVisible(false);
}

void xConnectorObj::generateShape()
{
    if (pInObj && pOutObj)
    {
        QPen p(Qt::black);
        p.setWidth(2);
        pGrpItem = new xGroupItem();
        pEndGrp = new QGraphicsItemGroup();
        pEndItem = new QGraphicsPathItem();
        pEllipseItem = new QGraphicsEllipseItem;

        pEndGrp->addToGroup(pEllipseItem);
        pEndGrp->addToGroup(pEndItem);
        pGrpItem->addToGroup(pEndGrp);
        pPathItem = new QGraphicsPathItem();
        pGrpItem->addToGroup(pPathItem);
        pPathItem->setPen(p);
        pEllipseItem->setPen(p);
        pEndItem->setPen(p);

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
        }
        else
        {
            pSelectionTimer->stop();
            pSelEffect->setEnabled(false);
            pEffect=new QGraphicsDropShadowEffect();
            pEffect->setBlurRadius(5);
            pEffect->setOffset(QPointF(3,3));
            if (pGrpItem) pGrpItem->setGraphicsEffect(pEffect);
        }
    }
}


xConnectorObj::~xConnectorObj()
{
    if (pGrpItem)
    {
        pGrpItem->scene()->removeItem(pGrpItem);
    }
}

void xConnectorObj::update()
{
    createPath();

    for (int i=0;i<_arrowLst.count();++i)
        _arrowLst[i]->setVisible(_paramMp["show arrows"]._value.toBool());
}

void xConnectorObj::save(QDataStream& d, bool _explicit)
{
    xAbstractBasisObj::save(d,_explicit);
    d << _id << _paramConnection << pInObj->id() << pOutObj->id();
}
