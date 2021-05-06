#include "xVGraphWdgt.h"
#include <QGraphicsPathItem>


void xVDataSeries::drawSeries()
{}

void xVAxis::drawAxis()
{}

xVGraphWdgt::xVGraphWdgt(QWidget*parent):QGraphicsView(parent)
{
    setScene(new QGraphicsScene());
    setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing|QPainter::TextAntialiasing);

    _paramMp["frame color"]._id=1;
    _paramMp["frame color"]._value=QColor(Qt::black);
    _paramMp["outline color"]._id=2;
    _paramMp["outline color"]._value=QColor(Qt::red);
    _paramMp["background color"]._id=3;
    _paramMp["background color"]._value=QColor(Qt::white);
}

void xVGraphWdgt::resizeEvent(QResizeEvent* e)
{
    QGraphicsView::resizeEvent(e);
    // update Scene Rect
    setSceneRect(contentsRect());
    updateGraph();
}

void xVGraphWdgt::KSlot(const SIG_TYPE& type,void* pData)
{
    switch (type)
    {
    default:
        // handled
        break;
    }
}

void xVGraphWdgt::attachAxisToSeries(xVAxis* pAxis,xVDataSeries* pDataSeries)
{}
void xVGraphWdgt::addSeries(xVDataSeries* pDataSeries)
{}
void xVGraphWdgt::removeAxis(xVAxis* pAxis)
{}
void xVGraphWdgt::removeSeries(xVDataSeries* pDataSeries)
{}
void xVGraphWdgt::updateGraph()
{
    scene()->setBackgroundBrush(QBrush(_paramMp["frame color"]._value.value<QColor>()));
    if (!pBackgroundItem)
    {
        pBackgroundItem = new QGraphicsPathItem();
        scene()->addItem(pBackgroundItem);
        pBackgroundItem->setZValue(-100);
    }

    QPainterPath path;
    path.addRoundedRect(QRectF(10,10,sceneRect().width()-20,sceneRect().height()-20),10,10);
    pBackgroundItem->setPath(path);
    pBackgroundItem->setPen(QPen(_paramMp["outline color"]._value.value<QColor>()));
    pBackgroundItem->setBrush(QBrush(_paramMp["background color"]._value.value<QColor>()));
}
