#ifndef XVHISTONODEITEM_H
#define XVHISTONODEITEM_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <xVTypes.h>
#include <QColor>
#include <QPen>

class xVHistoNodeItem:public QObject,public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    enum HNI_MODE
    {
        HNIM_FIXED      = 0x00,
        HNIM_VERTICAL   = 0x01,
        HNIM_HORIZONTAL = 0x02,
        HNIM_DELETABLE  = 0x04,
        HNIM_COLOR      = 0x08
    };

    xVHistoNodeItem(int nr,int mode,QObject * parent=nullptr);
    void setLimits(const QPointF& lb,const QPointF& hb);
    void setColor(const QColor& col);
    void setLineColor(const QColor& col);
    int nr(){return _nr;}
    QColor color(){return col;}
    void setShapeColor(QColor col){setPen(QPen(col,2));}
    void setHighlightColor(QColor col){_highlightColor=col;}
signals:
    void moved(QPointF,const int&,xVHistoNodeItem*);
    void aboutToBeKilled(xVHistoNodeItem*);
    void colorChanged(xVHistoNodeItem*);
protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    QPointF _lowLimit=QPointF(-1,-1);
    QPointF _highLimit=QPointF(-1,-1);
protected:
    int _nr;
    int _mode=HNIM_FIXED;
    QColor col=QColor::fromRgbF(0,0,1,1);
    QColor _highlightColor = Qt::yellow;
    QColor _color;
};

#endif // XVHISTONODEITEM_H
