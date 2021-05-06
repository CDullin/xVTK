#include "xVTypes.h"
#include "xVDashboardView.h"
#include "xVGraphicsScene.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QScrollBar>

xVDashboardView::xVDashboardView(QWidget* parent):QGraphicsView(parent)
{
    xVGraphicsScene *pScene=new xVGraphicsScene();
    // repair this
    connect(pScene,SIGNAL(dblClicked()),this,SLOT(deselectAll()));
    connect(pScene,SIGNAL(rDblClicked()),this,SLOT(rDblClickInScene()));

    setScene(pScene);
    scene()->setSceneRect(QRectF(0,0,800,500));
    setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing);


    QPixmap pix(QString("://images/pattern_%1.png").arg(_dashBoardCreateID%10));
    pix=pix.scaledToWidth(200,Qt::SmoothTransformation);
    scene()->setBackgroundBrush(QBrush(pix));

    pMapView = new QGraphicsView(parentWidget());
    pMapView->raise();
    pMapView->setScene(new QGraphicsScene());
    pMapView->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing);

    connect(pScene,SIGNAL(sceneRectChanged(const QRectF &)),this,SLOT(updateMap()));
    connect(pScene,SIGNAL(changed(const QList<QRectF>&)),this,SLOT(updateMap()));
}

void xVDashboardView::rDblClickInScene()
{
    emit KSignal(ST_RDBL_CLICK_IN_SCENE);
}

void xVDashboardView::deselectAll()
{
    emit KSignal(ST_DESELECT_ALL_OBJS);
}

void xVDashboardView::KSlot(const SIG_TYPE& type,void *pData)
{}

void xVDashboardView::switchPosition()
{
    _topRight=!_topRight;
    updateMap();
}

void xVDashboardView::moveScrollArea(QPointF p)
{
    //scroll(p.x(),p.y());
}

void xVDashboardView::updateMap()
{
    disconnect(scene(),SIGNAL(sceneRectChanged(const QRectF &)),this,SLOT(updateMap()));
    disconnect(scene(),SIGNAL(changed(const QList<QRectF>&)),this,SLOT(updateMap()));

    if (pMapGrpItem==nullptr)
    {
        QPixmap pix(width()/20,height()/20);
        pix.fill(Qt::white);
        pMapPixItem = new QGraphicsPixmapItem(pix);

        pMapGrpItem = new xGroupItem();
        pMapGrpItem->addToGroup(pMapPixItem);
        pMapGrpItem->setZValue(500);

        pFrameItem = new QGraphicsRectItem(pMapPixItem);
        pFrameItem->setPen(QPen(Qt::white,2));
        pFrameItem->setBrush(QBrush(Qt::NoBrush));
        //pMapGrpItem->addToGroup(pFrameItem);

        pViewportRectItem = new xRectItem();
        pViewportRectItem->setPen(QPen(Qt::red,10));
        pViewportRectItem->setBrush(QBrush(Qt::NoBrush));
        pViewportRectItem->setFlags(QGraphicsItem::ItemIsMovable);
        pViewportRectItem->setZValue(550);
        pViewportRectItem->setPos(0,0);
        pMapGrpItem->addToGroup(pViewportRectItem);
        connect(pViewportRectItem,SIGNAL(moved(QPointF)),this,SLOT(moveScrollArea(QPointF)));
        connect(pMapGrpItem,SIGNAL(selected()),this,SLOT(switchPosition()));

        QGraphicsDropShadowEffect *pEffect=new QGraphicsDropShadowEffect();
        pEffect->setOffset(3,3);
        pEffect->setColor(Qt::black);
        pEffect->setBlurRadius(5);
        pMapGrpItem->setGraphicsEffect(pEffect);

        pMapView->scene()->addItem(pMapGrpItem);
        pMapGrpItem->setHandlesChildEvents(true);
    }

    disconnect(pViewportRectItem,SIGNAL(moved(QPointF)),this,SLOT(moveScrollArea(QPointF)));

    if (parentWidget()!=0 && pMapView->parentWidget()==0) pMapView->setParent(this);

    // adapt scene rect to content
    if (scene() && scene()->items().count()>0) scene()->setSceneRect(QRectF());
    // visible rect in viewport
    QRectF visRect=mapToScene(viewport()->geometry()).boundingRect();
    // rect of the scene
    QRectF _sceneRect = scene()->sceneRect();
    QRectF unitRect = visRect.united(_sceneRect);
    QPointF trans = unitRect.topLeft()-_sceneRect.topLeft();

    QPixmap pix(unitRect.width(),unitRect.height());
    pix.fill(QColor(0,0,0,0));
    QPainter pain(&pix);
    QRect r(_sceneRect.toRect());
    r.translate(-visRect.left(),-visRect.top());
    pain.setTransform(QTransform::fromTranslate(-trans.x(),-trans.y()));
    render(&pain,QRect(),r);
    pain.end();
    pMapPixItem->setPixmap(pix);
    pFrameItem->setRect(0,0,pMapPixItem->pixmap().width(),pMapPixItem->pixmap().height());
    pViewportRectItem->setRect(QRectF(0,0,visRect.width(),visRect.height()));
    pViewportRectItem->setPos(-unitRect.topLeft().x()+visRect.topLeft().x(),-unitRect.topLeft().y()+visRect.topLeft().y());
    //pViewportRectItem->setPos(visRect.topLeft());
    float _scalFac = 200.0f/ (float)pMapPixItem->pixmap().width();
    pMapGrpItem->setScale(_scalFac);
    if (_topRight)
        pMapView->setGeometry(width()-200-10-20*verticalScrollBar()->isVisible(),10,200+10,(float)(pMapPixItem->pixmap().height())*_scalFac+10);
    else
        pMapView->setGeometry(10,height()-10-20*horizontalScrollBar()->isVisible()-(float)(pMapPixItem->pixmap().height())*_scalFac,200+10,(float)(pMapPixItem->pixmap().height())*_scalFac+10);

    pMapView->scene()->setSceneRect(0,0,200,(float)(pMapPixItem->pixmap().height())*_scalFac);
    pMapView->setVisible(!visRect.contains(_sceneRect));
    pMapGrpItem->setBoundingRectSize(QRectF());

    connect(pViewportRectItem,SIGNAL(moved(QPointF)),this,SLOT(moveScrollArea(QPointF)));

    connect(scene(),SIGNAL(sceneRectChanged(const QRectF &)),this,SLOT(updateMap()));
    connect(scene(),SIGNAL(changed(const QList<QRectF>&)),this,SLOT(updateMap()));
}
