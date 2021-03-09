#include "xVCustomGraphicItems.h"
#include <QCursor>
#include <QApplication>
#include <QTimer>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>

void xEllipseItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    _hovered = true;
    QPen p(Qt::yellow);
    p.setWidth(2);
    setPen(p);
    update();

    setCursor(Qt::PointingHandCursor);

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

