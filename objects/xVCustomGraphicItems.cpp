#include "xVCustomGraphicItems.h"
#include <QCursor>
#include <QApplication>
#include <QTimer>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsScene>

void xRectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::CrossCursor);
    QGraphicsRectItem::hoverEnterEvent(event);
}
void xRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    QGraphicsRectItem::hoverLeaveEvent(event);
}
void xRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->pos();
    QPointF p2 = mapToScene(pos+QPointF(rect().width(),rect().height()));

    float dx = p2.x()-pos.x();
    float dy = p2.y()-pos.y();

    if (pos.x()<0) pos.setX(0);
    if (pos.y()<0) pos.setY(0);
    /*
    if (p2.x()>scene()->sceneRect().width()) pos.setX(scene()->sceneRect().right()-dx);
    if (p2.y()>scene()->sceneRect().height()) pos.setY(scene()->sceneRect().bottom()-dy);
*/
    //emit moved(event->pos()-event->lastPos());
    setPos(pos);
    //QGraphicsRectItem::mouseMoveEvent(event);
}

void xGraphicsPathItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit hoverEnter();
}

void xEllipseItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    _hovered = true;
    QPen p(Qt::yellow);
    p.setWidth(2);
    setPen(p);
    update();

    setCursor(Qt::PointingHandCursor);
    emit hoverEnter();

    return QGraphicsEllipseItem::hoverEnterEvent(event);
}
void xEllipseItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    _hovered = false;
    QPen p(_activated ? Qt::red : Qt::black);
    p.setWidth(2);
    setPen(p);
    update();

    setCursor(Qt::ArrowCursor);
    return QGraphicsEllipseItem::hoverLeaveEvent(event);
}
void xEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _activated = !_activated;
    QPen p(_hovered ? Qt::yellow : _activated ? Qt::red : Qt::black);
    p.setWidth(2);
    setPen(p);
    update();
    emit activated();
    return QGraphicsEllipseItem::mousePressEvent(event);
}

xScrollTxtItem::xScrollTxtItem(const float& width,const QString& txt,QGraphicsItem *parent) : QObject(),QGraphicsItemGroup(parent)
{
    _txt = txt;
    _width = width;
    _enabled = true;
    _selected = false;
    _highlighted = false;
    _currentTxtPos = 0;

    QPainterPath path;
    path.addRoundedRect(QRectF(0,0,width,18),5,5);
    pBackgroundItem = new QGraphicsPathItem(path,this);
    pBackgroundItem->setPen(QColor(200,200,0));
    pBackgroundItem->setZValue(-1);

    pTxtDisplayItem = new QGraphicsSimpleTextItem(_txt,this);
    pTxtDisplayItem->setText(_txt);
    QFont f;f=qApp->font();f.setPointSizeF(12);
    pTxtDisplayItem->setFont(f);
    pTxtDisplayItem->setPen(QColor(100,100,0));
    pTxtDisplayItem->setBrush(QColor(100,100,0));
    pTxtDisplayItem->setPos(5,0);

    _rect = QRectF(0,0,width,18);

    pScrollTimer = new QTimer(this);
    pScrollTimer->setInterval(180);
    pScrollTimer->setSingleShot(false);
    pScrollTimer->start();
    connect(pScrollTimer,SIGNAL(timeout()),this,SLOT(scrollTimerTimeout()));
    setAcceptedMouseButtons(Qt::AllButtons);
    setScale(0.75);
    setHighLighted(false);
    scrollTimerTimeout();
}

QRectF xScrollTxtItem::boundingRect() const
{
    return _rect;
}

void xScrollTxtItem::setText(const QString &s)
{
    _txt = s;
    _currentTxtPos = 0;
    scrollTimerTimeout();
}

void xScrollTxtItem::setItemSelected(const bool &b)
{
    _selected = b;
    if (_selected)
    {
        pBackgroundItem->setBrush(QColor(100,100,0));
        pTxtDisplayItem->setPen(QColor(0,0,0));
    }
    else
    {
        pBackgroundItem->setBrush(Qt::NoBrush);
        pTxtDisplayItem->setPen(QColor(100,100,0));
    }
}

void xScrollTxtItem::setHighLighted(const bool &b)
{
    _highlighted=b;
    if (_highlighted)
        pBackgroundItem->setPen(QColor(255,255,255));
    else
        pBackgroundItem->setPen(Qt::NoPen);
}

void xScrollTxtItem::scrollTimerTimeout()
{
    QFontMetrics fm(pTxtDisplayItem->font());
    if (fm.boundingRect(_txt).width()>_width-10)
    {
        _currentTxtPos+=_direction;
        if (_currentTxtPos<0) {_currentTxtPos=0;_direction=1;}
        if (_currentTxtPos>_txt.length()) {_direction=-1;}
        int rb=_txt.length();
        while (fm.boundingRect(_txt.mid(_currentTxtPos,rb)).width()>_width-10)
        {
            --rb;
        }
        pTxtDisplayItem->setText(_txt.mid(_currentTxtPos,rb));
        pTxtDisplayItem->update();
    }
    else
    {
        pTxtDisplayItem->setText(_txt);
        pTxtDisplayItem->update();
    }
}

QRectF xGroupItem::boundingRect() const
{
    return _boundingRect;
}

xGroupItem::xGroupItem(QGraphicsItem *parent):QObject(),QGraphicsItemGroup(parent)
{
};

void xGroupItem::setBoundingRectSize(QRectF _br){_boundingRect=_br;}


void xGroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItemGroup::mouseMoveEvent(mouseEvent);
    if (mouseEvent->pos()!=mouseEvent->lastPos())
    {
        emit moved();
    }
}

void xGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsItemGroup::mouseReleaseEvent(mouseEvent);
    if (mouseEvent->button()==Qt::LeftButton) emit placed();
}

void xGroupItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button()==Qt::RightButton) emit selected();
}
