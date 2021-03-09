#include "xVHistoNodeItem.h"
#include "xrcolorpickdlg.h"

#include <QCursor>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <stdlib.h>

using namespace std;

xVHistoNodeItem::xVHistoNodeItem(int n,int m,QObject * parent):QObject(parent),QGraphicsEllipseItem()
{
    setPen(QPen(Qt::white,2));
    setBrush(QBrush(col));
    setRect(QRectF(-5,-5,10,10));

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);

    QGraphicsDropShadowEffect *pEffect=new QGraphicsDropShadowEffect();
    pEffect->setColor(Qt::black);
    pEffect->setOffset(3,3);
    pEffect->setBlurRadius(1);

    setGraphicsEffect(pEffect);

    _nr=n;
    _mode=m;
}

void xVHistoNodeItem::setColor(const QColor &c)
{
    col=c;
    setBrush(col);
}

void xVHistoNodeItem::setLineColor(const QColor &c)
{
    setPen(QPen(c,2));
}

void xVHistoNodeItem::setLimits(const QPointF& lb,const QPointF& hb)
{
    _lowLimit=lb;
    _highLimit=hb;
}

void xVHistoNodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsEllipseItem::hoverMoveEvent(event);
}

void xVHistoNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event){

    _color = pen().color();
    if (_mode & HNIM_VERTICAL)
        setCursor(QCursor(Qt::SizeVerCursor));
    if (_mode & HNIM_HORIZONTAL)
        setCursor(QCursor(Qt::SizeHorCursor));
    if ((_mode & HNIM_VERTICAL)>0 && ((_mode & HNIM_HORIZONTAL)>0))
        setCursor(QCursor(Qt::SizeAllCursor));
    if (_mode & HNIM_DELETABLE)
        setPen(QPen(_highlightColor,2));
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void xVHistoNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setPen(QPen(_color,2));
    setCursor(QCursor(Qt::ArrowCursor));
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void xVHistoNodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF actualPos=pos();
    QGraphicsEllipseItem::mouseMoveEvent(event);
    QPointF p=pos();
    if (_lowLimit!=QPointF(-1,-1) || _highLimit!=QPointF(-1,-1))
    {
        p.setX(max(_lowLimit.x(),min(_highLimit.x(),p.x())));
        p.setY(max(_lowLimit.y(),min(_highLimit.y(),p.y())));
    }
    if (_mode & HNIM_VERTICAL) actualPos.setY(p.y());
    if (_mode & HNIM_HORIZONTAL) actualPos.setX(p.x());

    setPos(actualPos);
    emit moved(actualPos,_nr,this);
}

void xVHistoNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (_mode & HNIM_COLOR)
    {
        xrColorPickDlg dlg;
        dlg.setColor(col);
        if (dlg.exec()==QDialog::Accepted)
        {
            col = dlg.currentColor();
            setBrush(col);
            emit colorChanged(this);
        }
    }

    return QGraphicsEllipseItem::mouseDoubleClickEvent(event);
}


void xVHistoNodeItem::keyPressEvent(QKeyEvent *pKEvent)
{
    if ((_mode & HNIM_DELETABLE)>0 && (pKEvent->key()==Qt::Key_Delete))
        emit aboutToBeKilled(this);

    QGraphicsEllipseItem::keyPressEvent(pKEvent);
}
