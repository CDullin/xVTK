#include "xVHistoPixmapItem.h"
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

xVHistoPixmapItem::xVHistoPixmapItem(QObject* parent):QObject(parent),QGraphicsPixmapItem()
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(QGraphicsItem::ItemIsMovable,false);
    setHandlesChildEvents(false);
}

void xVHistoPixmapItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    emit cursorPos(event->pos());
    QGraphicsPixmapItem::hoverMoveEvent(event);
}
void xVHistoPixmapItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::CrossCursor));
    QGraphicsPixmapItem::hoverEnterEvent(event);
}
void xVHistoPixmapItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}
void xVHistoPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit dblClick(event->pos());
    return QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

