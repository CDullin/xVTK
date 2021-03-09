#ifndef XVHISTOHVALUEAXIS_H
#define XVHISTOHVALUEAXIS_H

#include <QObject>
#include <QGraphicsItemGroup>
#include "xVHistoDlg_types.h"
#include "xVHistoPixmapItem.h"

class xVHistoDlg;

class xVHistoHValueAxis:public QObject,public QGraphicsItemGroup
{
    Q_OBJECT
public:
    xVHistoHValueAxis(int i,QObject *parent=nullptr);
    void setReferences(xVHISTO_PIXMAP*,xVHistoPixmapItem*,xVHistoDlg*);
    void updateAxis();
    void setColor(QColor col){_color=col;updateAxis();}
    void setGridColor(QColor col){_gridColor=col;updateAxis();}
    void setGridVisible(bool b){_showGrid=b;updateAxis();}
protected:
    QGraphicsPathItem *pArrowItem = nullptr;
    QGraphicsPathItem *pTickItem = nullptr;                // ticks and maingrid
    QList <QGraphicsSimpleTextItem*> _labelLst;         // ticks and maingrid

    xVHISTO_PIXMAP *pRefHisto = nullptr;
    xVHistoPixmapItem *pRefHistoPixItem = nullptr;
    xVHistoDlg* pRefDlg=nullptr;
    int _id;
    QColor _color = Qt::white;
    QColor _gridColor = Qt::lightGray;
    bool _showGrid = true;
};

#endif // XVHISTOHVALUEAXIS_H
