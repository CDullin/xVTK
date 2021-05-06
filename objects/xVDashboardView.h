#ifndef XVDASHBOARDVIEW_H
#define XVDASHBOARDVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include "xVCustomGraphicItems.h"
#include "xVTypes.h"

class xVDashboardView: public QGraphicsView
{
    Q_OBJECT
public:
    xVDashboardView(QWidget *parent = nullptr);

public slots:
    void updateMap();
    void moveScrollArea(QPointF);
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);

protected slots:
    void switchPosition();
    void deselectAll();
    void rDblClickInScene();

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

protected:
    QGraphicsView *pMapView = nullptr;
    xGroupItem *pMapGrpItem = nullptr;
    QGraphicsPixmapItem *pMapPixItem = nullptr;
    QGraphicsRectItem *pFrameItem = nullptr;
    xRectItem *pViewportRectItem = nullptr;

    bool _topRight = true;
};

#endif // XVDASHBOARDVIEW_H
