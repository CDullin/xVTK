#include "xVCallDashboardObj.h"
#include <QPainter>

xVCallDashboardObj::xVCallDashboardObj(const QString& txt):xVObj_Basics()
{
    _type = xVOT_CALL_DASHBOARD;
    _description="Used to execute another dashboard.";
    generateShape();

    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["file name"]._value = QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,::_settings["last dashboard file"]._value.toString()));
    _paramMp["file name"]._id = 1;

    generateShape();

    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(120,12.5+25);
    pGrpItem->addToGroup(pOutputCon->item());

    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(52,-10);
    pGrpItem->addToGroup(pParamInputCon->item());

    xConnector* pInputCon=new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(0,12.5+25);
    pGrpItem->addToGroup(pInputCon->item());

    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVCallDashboardObj::xVCallDashboardObj(QDataStream& d):xVObj_Basics(d)
{
    _type = xVOT_CALL_DASHBOARD;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}

void xVCallDashboardObj::paramModified(const QString& txt)
{
    xVObj_Basics::paramModified(txt);
    if (_paramMp["file name"]._value.value<QFileInfo>().exists())
    {
        QFile f(_paramMp["file name"]._value.value<xFileName>()._fileName);
        if (f.open(QFile::ReadOnly))
        {
            QDataStream d(&f);
            QPixmap pix; d >> pix;
            pix=pix.scaled(110,50,Qt::KeepAspectRatio);
            pPixItem->setPixmap(pix);
            f.close();
        }
    }
}

void xVCallDashboardObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        // load dashboard if not already loaded
        QFileInfo info(_paramMp["file name"]._value.value<xFileName>()._fileName);
        if (info.exists() && info.isFile())
        {
            QFile f(_paramMp["file name"]._value.value<xFileName>()._fileName);
            if (f.open(QFile::ReadOnly))
            {
                QDataStream d(&f);
                QPixmap pix; d >> pix;
                pix=pix.scaled(110,50,Qt::KeepAspectRatio);
                pPixItem->setPixmap(pix);
                f.close();
            }

            int _currendDashboardID = _currentDashBoard;

            emit KSignal(ST_LOAD_DASHBOARD,new QString(info.absoluteFilePath()));
            if (_currentDashBoard!=_currendDashboardID)
            {
                // switch to dashboard
                emit KSignal(ST_RUN);
                // run dashboard
                emit KSignal(ST_ACTIVATE_DASHBOARD,new int(_currendDashboardID));
                emit KSignal(ST_DASHBOARD_INFO_MODIFIED);
                // switch back to this dashboard
                setStatus(OS_VALID);
            }
            else
            {
                emit KSignal(ST_ERROR_MSG,new QString(QString("Dashboard %1 was not loaded successfully").arg(_paramMp["file name"]._value.value<QFileInfo>().absoluteFilePath())));
                setStatus(OS_ERROR);
            }
        }
        else
        {
            emit KSignal(ST_ERROR_MSG,new QString(QString("Dashboard %1 does not exists").arg(_paramMp["file name"]._value.value<QFileInfo>().absoluteFilePath())));
            setStatus(OS_ERROR);
        }
    }
}

void xVCallDashboardObj::generateShape()
{
    QPainterPath path;
    path.addRoundedRect(QRect(0,0,120,75),10,10);

    QPen pen(Qt::black);
    QRadialGradient rg(10,10, 100, 0, 1, 5);
    rg.setColorAt(0,QColor(200,200,200));
    rg.setColorAt(1,QColor(100,100,100));

    QBrush brush(rg);
    pShapeItem = new xGraphicsPathItem(path);
    connect(pShapeItem,SIGNAL(hoverEnter()),this,SLOT(updateDescToolTip()));
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(110,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);

    QPixmap pix(110,50);
    pix.fill(QColor(255,255,255,50));
    QPainter pain(&pix);
    pain.setPen(Qt::red);
    pain.drawText(QRectF(0,0,110,50),Qt::AlignCenter,"undefined");
    pPixItem = new QGraphicsPixmapItem(pix);
    pPixItem->setPos(5,20);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->addToGroup(pPixItem);
    pGrpItem->setBoundingRectSize(QRectF(-22,-15,168,93));
/*
    pBoundingRectItem = new QGraphicsRectItem(pGrpItem->boundingRect());
    pBoundingRectItem->setPen(QPen(Qt::white,2));
    pBoundingRectItem->setBrush(QBrush(Qt::NoBrush));
    pGrpItem->addToGroup(pBoundingRectItem);
*/
    pStatusItem->moveBy(15,0);
    pGrpItem->addToGroup(pStatusItem);

    pGrpItem->setFlags(QGraphicsItem::ItemIsMovable);
    pEffect=new QGraphicsDropShadowEffect();
    pEffect->setBlurRadius(5);
    pEffect->setOffset(QPointF(3,3));
    pGrpItem->setGraphicsEffect(pEffect);

    pGrpItem->setAcceptHoverEvents(true);
    pGrpItem->setAcceptedMouseButtons(Qt::AllButtons);
    pGrpItem->setHandlesChildEvents(false);
}
