#include "xVHistoVLine.h"
#include <QCursor>
#include <QPen>
#include <QGraphicsSceneMouseEvent>

#include <stdlib.h>
#include <math.h>

using namespace std;

xVHistoVLine::xVHistoVLine(const int& i,QObject* parent):QObject(parent),QGraphicsLineItem()
{
    _id=i;
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlags(QGraphicsItem::ItemIsMovable);
}

void xVHistoVLine::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsLineItem::hoverMoveEvent(event);
}
void xVHistoVLine::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    _color = pen().color();
    setCursor(QCursor(Qt::SizeHorCursor));
    setPen(QPen(_highlightColor,2));
    QGraphicsLineItem::hoverEnterEvent(event);
}
void xVHistoVLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    setPen(QPen(_color,2));
    QGraphicsLineItem::hoverLeaveEvent(event);
}
void xVHistoVLine::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit dblClick(event->pos(),_id);
    QGraphicsLineItem::mouseDoubleClickEvent(event);
}
void xVHistoVLine::setLimits(const int &lb, const int &hb)
{
    _lowLimit=lb;
    _highLimit=hb;
}
void xVHistoVLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pp=mapToParent(event->pos());

    pp.setX(max(_lowLimit,min(_highLimit,pp.x())));
    pp.setY(pos().y());
    setPos(pp);
    emit moved(pp,_id);
}
