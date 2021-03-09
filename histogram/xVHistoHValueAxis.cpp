#include "xVHistoHValueAxis.h"

#include <QPen>
#include <QBrush>
#include "xvhistodlg.h"

xVHistoHValueAxis::xVHistoHValueAxis(int i,QObject *parent):QObject(parent),QGraphicsItemGroup()
{
    _id=i;
    _color=Qt::white;
}
void xVHistoHValueAxis::setReferences(xVHISTO_PIXMAP* pH,xVHistoPixmapItem* pPi,xVHistoDlg* pD)
{
    pRefHisto=pH;
    pRefHistoPixItem=pPi;
    pRefDlg=pD;

    updateAxis();
}

void xVHistoHValueAxis::updateAxis()
{
    if (!pArrowItem)
    {
        pArrowItem = new QGraphicsPathItem();
        pArrowItem->setBrush(QBrush(QColor(_color)));
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

    double w = pRefHistoPixItem->pixmap().width();

    QPainterPath path;
    path.moveTo(QPointF(-10,0));
    path.lineTo(QPointF(w+3,0));
    path.lineTo(QPointF(w+3,-2));
    path.lineTo(QPointF(w+13,0));
    path.lineTo(QPointF(w+3,+2));
    path.lineTo(QPointF(w+3,0));

    QPainterPath tickPath;

    for (int i=0;i<_labelLst.count();++i)
    {
        removeFromGroup(_labelLst[i]);
        delete _labelLst[i];
    }
    _labelLst.clear();

    // calculate optimal tick count
    double br = pRefHisto->_dispMax-pRefHisto->_dispMin;
    double ex = log(br)/log(10);
    double exp = ex*10.0-floor(ex)*10.0;
    exp < 2 ? ex=floor(ex-1) : ex=floor(ex);
    double _inc = pow(10,ex);
    double swx = ceil(pRefHisto->_dispMin/_inc)*_inc;
    bool ok;
    double h=3+pRefHistoPixItem->pixmap().height()+5;
    if (!_showGrid) h=8;
    double wx;
    float mappedV;
    for (wx=swx;wx<pRefHisto->_dispMax;wx+=_inc)
    {
        if (_id==xVHistoDlg::HLM_HOR_AXIS_ALL) mappedV=pRefDlg->mapGValToAllPixmap(wx,ok);
        else mappedV=pRefDlg->mapGValToZoomPixmap(wx,ok);
        tickPath.moveTo(QPointF(mappedV,+3));
        tickPath.lineTo(QPointF(mappedV,-h));

        float val = wx/_inc;

        QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("%1").arg(wx/_inc,0,'f',0));
        addToGroup(pSimpleTxtItem);
        _labelLst.append(pSimpleTxtItem);
        pSimpleTxtItem->setPos(mappedV-pSimpleTxtItem->boundingRect().width()/2.0,5);
        pSimpleTxtItem->setPen(QPen(_color));
        pSimpleTxtItem->setBrush(_color);
    }

    QGraphicsSimpleTextItem *pSimpleTxtItem = new QGraphicsSimpleTextItem(QString("x10^%1").arg(ex));
    addToGroup(pSimpleTxtItem);
    _labelLst.append(pSimpleTxtItem);
    pSimpleTxtItem->setPos(w+3,5);
    pSimpleTxtItem->setPen(QPen(_color));
    pSimpleTxtItem->setBrush(_color);

    QPainterPath path2;
    _inc = pow(10,ex-1)*2;
    swx = ceil(pRefHisto->_dispMin/_inc)*_inc;
    for (wx=swx;wx<pRefHisto->_dispMax;wx+=_inc)
    {
        if (_id==xVHistoDlg::HLM_HOR_AXIS_ALL) mappedV=pRefDlg->mapGValToAllPixmap(wx,ok);
        else mappedV=pRefDlg->mapGValToZoomPixmap(wx,ok);
        tickPath.moveTo(QPointF(mappedV,+3));
        tickPath.lineTo(QPointF(mappedV,-h));
    }

    pArrowItem->setPath(path);
    pTickItem->setPath(tickPath);
}
