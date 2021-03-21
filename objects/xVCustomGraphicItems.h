#ifndef XVCUSTOMGRAPHICITEMS_H
#define XVCUSTOMGRAPHICITEMS_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

class xEllipseItem:public QObject,public QGraphicsEllipseItem
{
Q_OBJECT
public:
    xEllipseItem(const QRectF &rect, QGraphicsItem *parent = nullptr):QObject(),QGraphicsEllipseItem(rect,parent){
        setAcceptHoverEvents(true);
        setAcceptedMouseButtons(Qt::AllButtons);
    }
    void setActivated(bool b){
        _activated=b;
        QPen p(_hovered ? Qt::yellow : _activated ? Qt::red : Qt::black);
        p.setWidth(2);
        setPen(p);
        update();
    }
    bool isActivated(){return _activated;}
signals:
    void activated();
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
protected:
    bool _activated = false;
    bool _hovered = false;
};

class xGroupItem:public QObject,public QGraphicsItemGroup
{
    Q_OBJECT
public:
    xGroupItem(QGraphicsItem *parent = nullptr):QObject(),QGraphicsItemGroup(parent){};
    virtual QRectF boundingRect() const override;
    void setBoundingRectSize(QRectF _br){_boundingRect=_br;}
signals:
    void placed();
    void moved();
    void selected();
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    {
        QGraphicsItemGroup::mouseMoveEvent(mouseEvent);
        if (mouseEvent->pos()!=mouseEvent->lastPos())
            emit moved();
    }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    {
        QGraphicsItemGroup::mouseReleaseEvent(mouseEvent);
        emit placed();
    }
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override
    {
        QGraphicsItemGroup::mouseDoubleClickEvent(event);
        emit selected();
    }

    QRectF _boundingRect;
};

class xScrollTxtItem : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    explicit xScrollTxtItem(const float& width,const QString& txt="",QGraphicsItem *parent = nullptr);

public:
    virtual QRectF boundingRect() const override;

    void setPen(QPen p){if (pTxtDisplayItem) pTxtDisplayItem->setPen(p);}
    void setText(const QString&);
    bool isSelected(){return _selected;}
    virtual void setItemSelected(const bool&);
    bool highLighted(){return _highlighted;}
    void setHighLighted(const bool&);

protected slots:
    void scrollTimerTimeout();

signals:

private:
    int _direction = 1;
    bool _highlighted;
    bool _enabled;
    bool _selected;
    QString _txt;
    float _width;
    QRectF _rect;
    int _currentTxtPos=0;

    QGraphicsSimpleTextItem *pTxtDisplayItem = nullptr;
    QGraphicsPathItem *pBackgroundItem = nullptr;
    QTimer *pScrollTimer = nullptr;
};

#endif // XVCUSTOMGRAPHICITEMS_H
