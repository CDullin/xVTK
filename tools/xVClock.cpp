#include "xVClock.h"
#include <QResizeEvent>
#include <math.h>
#include <QPainterPath>
#include <QTimeZone>

xVClock::xVClock(QWidget *parent):QLabel(parent)
{
    setPixmap(generatePixmap());
    QTimer* pTimer=new QTimer(this);
    pTimer->setSingleShot(false);
    pTimer->setInterval(2000);
    connect(pTimer,SIGNAL(timeout()),this,SLOT(timeout()));
    pTimer->start();
}

void xVClock::setPixmap(const QPixmap& p)
{
    QLabel::setPixmap(p);
    pix=p;
}

void xVClock::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    pix=generatePixmap();
}

QPixmap xVClock::generatePixmap()
{
    int ma=margin();
    int w = width()-2*ma;
    int h = height()-2*ma;
    int s = std::min(w,h);
    float cx = (float)w/2.0f;
    float cy = (float)h/2.0f;

    QPixmap pix(w,h);
    pix.fill(QColor(0,0,0,0));

    QPainter pain(&pix);
    pain.setRenderHints(QPainter::Antialiasing);
    pain.setPen(Qt::black);
    QLinearGradient rg(QPointF(0,0),QPointF(10,10));
    rg.setColorAt(0,QColor(255,0,0));rg.setColorAt(1,QColor(0,0,200));
    //pain.setBrush(rg);

    for (int i=0;i<12;++i)
    {
        pain.drawEllipse(QRect(cx+cos(M_PI/6.0*i)*s*0.4f-3,cx+sin(M_PI/6.0*i)*s*0.4f-3-3,7,7));
    }
    pain.end();

    return pix;
}

void xVClock::timeout()
{   
    QDateTime dt=QDateTime::currentDateTime();
    dt.setTimeZone(QTimeZone("CET+1"));
    float hour=dt.time().hour() % 12;
    float min=dt.time().minute();
    float sec=dt.time().second();

    int ma=margin();
    int w = width()-2*ma;
    int h = height()-2*ma;
    int s = std::min(w,h);
    float cx = (float)w/2.0f;
    float cy = (float)h/2.0f;

    QPixmap p2(pix);
    QPainter pai(&p2);
    pai.setPen(Qt::white);
    pai.drawText(QRectF(cx-30,cy+25,60,20),Qt::AlignCenter,dt.toString("dd.MM.yy"));
    //pai.drawText(QRectF(cx-30,cy+25,60,20),Qt::AlignCenter,dt.toString("hh:mm:ss"));
    pai.end();

    QPainter pain2(&p2);
    pain2.setRenderHints(QPainter::Antialiasing);
    pain2.setPen(QPen(Qt::black,2));
    QPainterPath path2;
    path2.moveTo(-1,-3);
    path2.lineTo(-1,10);
    path2.lineTo(0,25);
    path2.lineTo(1,10);
    path2.lineTo(1,-3);
    path2.lineTo(-1,-3);
    //path.translate(cx,cy);
    QTransform t2;
    t2=t2.translate(cx,cx-3);
    t2=t2.rotateRadians(M_PI/30*(min-30));
    pain2.setTransform(t2);
    pain2.drawPath(path2);
    pain2.end();

    QPainter pain3(&p2);
    pain3.setRenderHints(QPainter::Antialiasing);
    pain3.setPen(QPen(Qt::black,2));
    QPainterPath path3;
    path3.moveTo(-1,-3);
    path3.lineTo(-1,5);
    path3.lineTo(0,18);
    path3.lineTo(1,5);
    path3.lineTo(1,-3);
    path3.lineTo(-1,-3);
    //path.translate(cx,cy);
    QTransform t3;
    t3=t3.translate(cx,cx-3);
    t3=t3.rotateRadians(M_PI/6*(hour+6));
    pain3.setTransform(t3);
    pain3.drawPath(path3);
    pain3.end();

    QPainter pain(&p2);
    pain.setRenderHints(QPainter::Antialiasing);
    pain.setPen(QPen(Qt::red,2));
    QPainterPath path;
    path.moveTo(-1,-3);
    path.lineTo(-1,20);
    path.lineTo(0,28);
    path.lineTo(1,20);
    path.lineTo(1,-3);
    path.lineTo(-1,-3);
    //path.translate(cx,cy);
    QTransform t;
    t=t.translate(cx,cx-3);
    t=t.rotateRadians(M_PI/30.0*(sec+30.0));
    pain.setTransform(t);
    pain.drawPath(path);
    pain.end();

    QLabel::setPixmap(p2);
}

