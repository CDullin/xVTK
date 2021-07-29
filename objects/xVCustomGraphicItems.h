#ifndef XVCUSTOMGRAPHICITEMS_H
#define XVCUSTOMGRAPHICITEMS_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QPen>
#include <math.h>
#include <QTimer>

class xRectItem: public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    xRectItem(QGraphicsItem *parent = nullptr):QObject(),QGraphicsRectItem(parent)
    {
        //setAcceptHoverEvents(true);
        //setAcceptedMouseButtons(Qt::AllButtons);
    }
signals:
    void moved(QPointF);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
};

class xGraphicsPathItem:public QObject,public QGraphicsPathItem
{
    Q_OBJECT
public:
    xGraphicsPathItem(const QPainterPath &path, QGraphicsItem *parent = nullptr):QObject(),QGraphicsPathItem(path,parent)
    {
        setAcceptHoverEvents(true);
    }
    xGraphicsPathItem(QGraphicsItem *parent = nullptr):QObject(),QGraphicsPathItem(parent)
    {
        setAcceptHoverEvents(true);
    }
signals:
    void hoverEnter();
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
};

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
    void hoverEnter();
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
    xGroupItem(QGraphicsItem *parent = nullptr);
    virtual QRectF boundingRect() const override;
    void setBoundingRectSize(QRectF _br);
signals:
    void placed();
    void moved();
    void selected();
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
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
