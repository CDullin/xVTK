#ifndef XVHISTOVLINE_H
#define XVHISTOVLINE_H

#include <QObject>
#include <QGraphicsLineItem>
#include <xVTypes.h>

class xVHistoVLine:public QObject,public QGraphicsLineItem
{
    Q_OBJECT
public:
    xVHistoVLine(const int& i,QObject *parent=nullptr);
    void setLimits(const int& lb,const int& hb);
    void setHighlightColor(QColor col){_highlightColor=col;}
signals:
    void moved(QPointF,const int&);
    void dblClick(QPointF, const int&);
protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    double _lowLimit;
    double _highLimit;
    int _id;

    QColor _color = Qt::red;
    QColor _highlightColor = Qt::yellow;
};

#endif // XVHISTOVLINE_H
