#ifndef XVHISTOPIXMAPITEM_H
#define XVHISTOPIXMAPITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <xVTypes.h>

class xVHistoPixmapItem:public QObject,public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    xVHistoPixmapItem(QObject *parent=nullptr);
signals:
    void cursorPos(QPointF);
    void dblClick(QPointF);
protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // XVHISTOPIXMAPITEM_H
