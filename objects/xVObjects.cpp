#include "xVObjects.h"
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QCursor>
#include <math.h>
#include <stdlib.h>
#include <QGraphicsScene>
#include <QApplication>

using namespace std;

xVObj_Basics::xVObj_Basics():xAbstractBasisObj(){

    pUpdateTimer = new QTimer();
    pUpdateTimer->setInterval(1000);
    pUpdateTimer->setSingleShot(true);
    connect(pUpdateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTimeOut()));

    _type=xVOT_INVALID;
    _paramMp["name"]._value="base class";
    _paramMp["name"]._id=0;
    pStatusItem = new QGraphicsEllipseItem(QRectF(0,0,10,10));
    pStatusItem->setPen(QPen(Qt::black));
    pStatusItem->setZValue(5);
    pStatusItem->moveBy(80,-14);
    pColEffect = new QGraphicsColorizeEffect();
    pColEffect->setStrength(1.0);
    pStatusItem->setGraphicsEffect(pColEffect);
    pColEffect->setEnabled(false);
    paAnimation = new QPropertyAnimation(pColEffect,"color",this);        // set parent is important
    paAnimation->setStartValue(QColor(200,200,200,0));
    paAnimation->setDuration(300);
    pAnimationTimer = new QTimer();
    pAnimationTimer->setInterval(1000);
    pAnimationTimer->setSingleShot(false);
    connect(pAnimationTimer,SIGNAL(timeout()),paAnimation,SLOT(start()));

    pSelEffect = new QGraphicsDropShadowEffect();
    pSelEffect->setColor(Qt::red);
    pSelEffect->setBlurRadius(30);
    pSelEffect->setOffset(QPointF(0,0));
    paAnimation2 = new QPropertyAnimation(pSelEffect,"color",this);
    paAnimation2->setStartValue(QColor(Qt::red));
    paAnimation2->setKeyValueAt(0.5,QColor(Qt::yellow));
    paAnimation2->setEndValue(QColor(Qt::red));
    paAnimation2->setDuration(800);
    pSelectionTimer = new QTimer();
    pSelectionTimer->setInterval(1000);
    pSelectionTimer->setSingleShot(false);
    connect(pSelectionTimer,SIGNAL(timeout()),paAnimation2,SLOT(start()));

    setStatus(OS_UPDATE_NEEDED);
};

xVObj_Basics::xVObj_Basics(QDataStream& d):xAbstractBasisObj(d)
{
    d >> _maxInput >> _maxOutput >> _status;

    quint16 v; d >> v;
    for (int i=0;i<v;++i)
    {
        xConnector *pCon=new xConnector(this,d);
        QPointF pnt; d >> pnt;
        pCon->item()->setPos(pnt);
        _connectorLst.append(pCon);
    }

    _type=xVOT_INVALID;
    pUpdateTimer = new QTimer();
    pUpdateTimer->setInterval(1000);
    pUpdateTimer->setSingleShot(true);
    connect(pUpdateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTimeOut()));
    pStatusItem = new QGraphicsEllipseItem(QRectF(0,0,10,10));
    pStatusItem->setPen(QPen(Qt::black));
    pStatusItem->setZValue(5);
    pStatusItem->moveBy(80,-14);
    pColEffect = new QGraphicsColorizeEffect();
    pColEffect->setStrength(1.0);
    pStatusItem->setGraphicsEffect(pColEffect);
    pColEffect->setEnabled(false);
    paAnimation = new QPropertyAnimation(pColEffect,"color",this);        // set parent is important
    paAnimation->setStartValue(QColor(200,200,200,0));
    paAnimation->setDuration(300);
    pAnimationTimer = new QTimer();
    pAnimationTimer->setInterval(1000);
    pAnimationTimer->setSingleShot(false);
    connect(pAnimationTimer,SIGNAL(timeout()),paAnimation,SLOT(start()));

    pSelEffect = new QGraphicsDropShadowEffect();
    pSelEffect->setColor(Qt::red);
    pSelEffect->setBlurRadius(30);
    pSelEffect->setOffset(QPointF(0,0));
    paAnimation2 = new QPropertyAnimation(pSelEffect,"color",this);
    paAnimation2->setStartValue(QColor(Qt::red));
    paAnimation2->setKeyValueAt(0.5,QColor(Qt::yellow));
    paAnimation2->setEndValue(QColor(Qt::red));
    paAnimation2->setDuration(800);
    pSelectionTimer = new QTimer();
    pSelectionTimer->setInterval(1000);
    pSelectionTimer->setSingleShot(false);
    connect(pSelectionTimer,SIGNAL(timeout()),paAnimation2,SLOT(start()));

    setStatus(OS_UPDATE_NEEDED);
}

int xVObj_Basics::countOf(xCONNECTOR_TYPE type)
{
    int res=0;
    for (int i=0;i<_connectorLst.count();++i)
        if (_connectorLst[i]->type()==type)
            res+=_connectorLst[i]->connectedObjects()->count();
    return res;
}

xConnector* xVObj_Basics::connectedToVObjWithType(xVObj_Basics* pVObj,const xCONNECTOR_TYPE& type)
{
    xConnector* pResCon=nullptr;
    for (int i=0;i<_connectorLst.count();++i)
        if (_connectorLst[i]->connectedTo(pVObj) && _connectorLst[i]->type()==type)
            pResCon=_connectorLst[i];
    return pResCon;
}

xConnector* xVObj_Basics::activeConnector()
{
    xConnector* pResCon=nullptr;
    for (int i=0;i<_connectorLst.count();++i)
        if (_connectorLst[i]->isActivated())
            pResCon=_connectorLst[i];
    return pResCon;
}


void xVObj_Basics::save(QDataStream &d, bool _explicit)
{
    xAbstractBasisObj::save(d,_explicit);
    d << _maxInput << _maxOutput << OS_UPDATE_NEEDED;
    d << (quint16)_connectorLst.count();
    for (int i=0;i<_connectorLst.count();++i)
    {
        _connectorLst[i]->save(d);
        d << _connectorLst[i]->item()->pos();
    }
}

void xVObj_Basics::paramModified(const QString& txt) {
    if (txt=="name" && pTxtItem && _paramMp.contains(txt))
    {
        pTxtItem->setText(_paramMp[txt]._value.toString());
        pTxtItem->update();
    }

    // if we have outputs we need to update them as well

    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
        if ((*it2)->type()==xCT_OUTPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                (*it)->paramModified("");

    emit parameterModified();
}


void xVObj_Basics::setStatus(const OBJ_STATUS &s)
{
    pAnimationTimer->stop();
    pColEffect->setEnabled(false);
    _status= s;
    QRadialGradient rg(5,5,5,2,2);
    switch (status())
    {
    case OS_UPDATE_NEEDED: rg.setColorAt(0,QColor(0,0,255));rg.setColorAt(1,QColor(0,0,100));break;
    case OS_RUNNING: rg.setColorAt(0,QColor(0,255,0));rg.setColorAt(1,QColor(0,0,0));
        paAnimation->setEndValue(QColor(Qt::green));
        pColEffect->setEnabled(true);
        pAnimationTimer->start();
        break;
    case OS_VALID: rg.setColorAt(0,QColor(0,255,0));rg.setColorAt(1,QColor(0,100,0));break;
    case OS_WARNING:
        rg.setColorAt(0,QColor(255,0,0));rg.setColorAt(1,QColor(200,200,200));
        paAnimation->setEndValue(QColor(Qt::yellow));
        pColEffect->setEnabled(true);
        pAnimationTimer->start();
        break;
    case OS_ERROR:
        rg.setColorAt(0,QColor(255,0,0));rg.setColorAt(1,QColor(100,0,0));
        paAnimation->setEndValue(QColor(Qt::red));
        pColEffect->setEnabled(true);
        pAnimationTimer->start();
        break;
    }
    pStatusItem->setBrush(QBrush(rg));
    qApp->processEvents();
}


void xVObj_Basics::setParamSelected(bool b)
{
    _selected = b;
    if (pShapeItem!=nullptr)
    {
        QPen p;
        if (_selected)
        {
            disconnect(pSelectionTimer,0,0,0);
            pSelEffect = new QGraphicsDropShadowEffect();
            pSelEffect->setColor(Qt::red);
            pSelEffect->setBlurRadius(30);
            pSelEffect->setOffset(QPointF(0,0));
            paAnimation2 = new QPropertyAnimation(pSelEffect,"color",this);
            paAnimation2->setStartValue(QColor(Qt::red));
            paAnimation2->setKeyValueAt(0.5,QColor(Qt::yellow));
            paAnimation2->setEndValue(QColor(Qt::red));
            paAnimation2->setDuration(800);
            if (pGrpItem) pGrpItem->setGraphicsEffect(pSelEffect);
            pSelEffect->setEnabled(true);
            pSelectionTimer->start();
            connect(pSelectionTimer,SIGNAL(timeout()),paAnimation2,SLOT(start()));
        }
        else
        {
            pSelectionTimer->stop();
            pSelEffect->setEnabled(false);
            pEffect=new QGraphicsDropShadowEffect();
            pEffect->setBlurRadius(5);
            pEffect->setOffset(QPointF(3,3));
            if (pGrpItem) pGrpItem->setGraphicsEffect(pEffect);
        }
        //pShapeItem->setPen(p);
        pShapeItem->update();
    }
}

xVObj_Basics::~xVObj_Basics()
{
    if (pGrpItem)
    {
        pGrpItem->scene()->removeItem(pGrpItem);
        /*
        delete pGrpItem;
        if (pShapeItem) delete pShapeItem;
        if (pTxtItem) delete pTxtItem;
        if (pIconBackgroundItem) delete pIconBackgroundItem;
        if (pIconItem) delete pIconItem;
        */
    }
}




