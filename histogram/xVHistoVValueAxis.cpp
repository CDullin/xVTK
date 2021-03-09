#include "xVHistoVValueAxis.h"

#include <QPen>
#include <QBrush>

xVHistoVValueAxis::xVHistoVValueAxis(int i,QObject *parent):QObject(parent),QGraphicsItemGroup()
{
    _id=i;
}
void xVHistoVValueAxis::setReferences(xVHISTO_PIXMAP* pH,xVHistoPixmapItem* pPi,xVHistoDlg* pD)
{
    pRefHisto=pH;
    pRefHistoPixItem=pPi;
    pRefDlg=pD;

    updateAxis();
}

void xVHistoVValueAxis::updateAxis()
{
    if (!pArrowItem)
    {
        pArrowItem = new QGraphicsPathItem();
        this->addToGroup(pArrowItem);
        pTickItem = new QGraphicsPathItem();
        this->addToGroup(pTickItem);
        pTickItem->setZValue(-2);
    }
    pArrowItem->setPen(QPen(_color));
    pArrowItem->setBrush(QBrush(_color));
    QPen p(_gridColor);
    p.setStyle(Qt::DashLine);
    pTickItem->setPen(p);

    double h = pRefHistoPixItem->pixmap().height();

    QPainterPath path;
    path.moveTo(QPointF(0,h+10));
    path.lineTo(QPointF(0,-3));
    path.lineTo(QPointF(-3,-3));
    path.lineTo(QPointF(0,-13));
    path.lineTo(QPointF(3,-3));
    path.lineTo(QPointF(0,-3));

    QPainterPath tickPath;

    for (int i=0;i<_labelLst.count();++i)
    {
        removeFromGroup(_labelLst[i]);
        delete _labelLst[i];
    }
    _labelLst.clear();

    // calculate optimal tick count
    double w=3+pRefHistoPixItem->pixmap().width()+5;
    if (!_showGrid) w=8;
    double hx;
    for (hx=0.2;hx<1;hx+=0.2)
    {
        tickPath.moveTo(QPointF(-3,h-hx*h));
        tickPath.lineTo(QPointF(w,h-hx*h));
        QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("%1").arg(hx,0,'f',1));
        addToGroup(pSimpleTxtItem);
        _labelLst.append(pSimpleTxtItem);
        pSimpleTxtItem->setPos(-5-pSimpleTxtItem->boundingRect().width(),h-hx*h-pSimpleTxtItem->boundingRect().height()/2);
        pSimpleTxtItem->setPen(QPen(_color));
        pSimpleTxtItem->setBrush(_color);
    }
    pArrowItem->setPath(path);
    pTickItem->setPath(tickPath);
}
