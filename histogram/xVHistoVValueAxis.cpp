#include "xVHistoVValueAxis.h"
#include "xvhistodlg.h"
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
    xVHistoDlg::HISTO_VALUE_MODE _valueMode=pRefDlg->valueMode();

    QPainterPath path;
    path.moveTo(QPointF(0,h+10));
    path.lineTo(QPointF(0,-3));
    path.lineTo(QPointF(-3,-3));
    path.lineTo(QPointF(0,-13));
    path.lineTo(QPointF(3,-3));
    path.lineTo(QPointF(0,-3));

    QPainterPath tickPath;

    double _visMax = -1;
    if (pRefHistoPixItem)
    {
        if (pRefHistoPixItem->property("title")=="all")
            _visMax = pRefDlg->all()->_valMax;
        if (pRefHistoPixItem->property("title")=="zoom")
            _visMax = pRefDlg->zoom()->_valMax;
    }
    double _maxVal = pRefDlg->histo()->info()._maxVal;
    if (_visMax>-1) _maxVal=_visMax;

    for (int i=0;i<_labelLst.count();++i)
    {
        removeFromGroup(_labelLst[i]);
        delete _labelLst[i];
    }
    _labelLst.clear();

    switch (_valueMode)
    {
    case xVHistoDlg::HVM_RELATIVE:
    {
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

    }
        break;
    case xVHistoDlg::HVM_ABSOLUTE:
    {
        // calculate meaningful start and end value
        double _exp = floor(log(_maxVal)/log(10));
        double _sections = floor(_maxVal / pow(10.0,_exp));
        if (_sections<=1) _exp-=1;
        double _inc = pow(10.0,_exp);

        // calculate optimal tick count
        double w=3+pRefHistoPixItem->pixmap().width()+5;
        if (!_showGrid) w=8;
        double hx;
        double j=0;
        for (hx=0;hx<=_maxVal;hx+=2*_inc)
        {
            tickPath.moveTo(QPointF(-3,h-hx/_maxVal*h));
            tickPath.lineTo(QPointF(w,h-hx/_maxVal*h));
            QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("%1").arg(j++,0,'f',1));
            addToGroup(pSimpleTxtItem);
            _labelLst.append(pSimpleTxtItem);
            pSimpleTxtItem->setPos(-5-pSimpleTxtItem->boundingRect().width(),h-hx/_maxVal*h-pSimpleTxtItem->boundingRect().height()/2);
            pSimpleTxtItem->setPen(QPen(_color));
            pSimpleTxtItem->setBrush(_color);
        }

        QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("x10^%1").arg(_exp));
        addToGroup(pSimpleTxtItem);
        _labelLst.append(pSimpleTxtItem);
        pSimpleTxtItem->setPos(-35,-20);
        pSimpleTxtItem->setPen(QPen(_color));
        pSimpleTxtItem->setBrush(_color);

    }
        break;
    case xVHistoDlg::HVM_LOG:
    {
        _maxVal=pow(10.0,_maxVal);
        // calculate meaningful start and end value

        // calculate optimal tick count
        double w=3+pRefHistoPixItem->pixmap().width()+5;
        if (!_showGrid) w=8;
        double hx;
        double j=0;
        double step=0.2;
        for (hx=0;pow(10,hx)<=_maxVal;hx+=step)
        {
            tickPath.moveTo(QPointF(-3,h-pow(10,hx)/_maxVal*h));
            tickPath.lineTo(QPointF(w,h-pow(10,hx)/_maxVal*h));
        }

        for (hx=0;pow(10,hx)<=_maxVal;hx+=1)
        {
            QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("%1").arg(hx,0,'f',1));
            addToGroup(pSimpleTxtItem);
            _labelLst.append(pSimpleTxtItem);
            pSimpleTxtItem->setPos(-5-pSimpleTxtItem->boundingRect().width(),h-pow(10.0,hx)/_maxVal*h-pSimpleTxtItem->boundingRect().height()/2);
            pSimpleTxtItem->setPen(QPen(_color));
            pSimpleTxtItem->setBrush(_color);
        }

        QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("log"));
        addToGroup(pSimpleTxtItem);
        _labelLst.append(pSimpleTxtItem);
        pSimpleTxtItem->setPos(-35,-20);
        pSimpleTxtItem->setPen(QPen(_color));
        pSimpleTxtItem->setBrush(_color);

    }
        break;
    }

    pArrowItem->setPath(path);
    pTickItem->setPath(tickPath);
}
