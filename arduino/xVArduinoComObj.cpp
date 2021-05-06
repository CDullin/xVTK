#include "xVArduinoComObj.h"
#include "xVArduinoConnectObj.h"
#include <QSerialPort>

xVArduinoComThread::xVArduinoComThread(xSerialPortPtr pPort):QThread()
{
    pSerial=pPort;
    _abb = false;
}

void xVArduinoComThread::run()
{
    while (!_abb)
    {
        // check if something to send
        // check if something needs to be received
        msleep(5);
    }
}

void xVArduinoComThread::stop()
{
    _abb = true;
}

void xVArduinoComThread::sendMsg(const QString& txt)
{
    _sendMutex.lock();
    _sendMsgLst.append(txt);
    _sendMutex.unlock();
}

QString xVArduinoComThread::receivedMsg()
{
    _receiveMutex.lock();
    QString msg=_receivedMsgLst.takeFirst();
    _receiveMutex.unlock();
    return msg;
}

int xVArduinoComThread::receivedMsgCount()
{
    return _receivedMsgLst.count();
}

xVArduinoComObj::xVArduinoComObj(const QString& txt):xVObj_Basics()
{
    _type=xVOT_ARDUINO_COM;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;

    generateShape();
    xConnector* pInputCon = new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(0,12.5);
    pGrpItem->addToGroup(pInputCon->item());
    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(95,12.5);
    pGrpItem->addToGroup(pOutputCon->item());
    xConnector* pParamInputCon=new xConnector(this);
    _connectorLst.append(pParamInputCon);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(52-7,-10);
    pGrpItem->addToGroup(pParamInputCon->item());

    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}
xVArduinoComObj::xVArduinoComObj(QDataStream& d):xVObj_Basics(d)
{
    _type=xVOT_ARDUINO_COM;
    _maxInput=1000;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}
void xVArduinoComObj::reset()
{
    setStatus(OS_UPDATE_NEEDED);
}

// generates a msg to be send via serial port
QString xVArduinoComObj::encode(const ARDUINO_COM_CODE& code,const xPROP_TYPE& prop)
{
    QString msg;
    return msg;
}
// retreives information from a msg and modifies output parameter or the global namespace
void xVArduinoComObj::decode(const QString& msg)
{}

void xVArduinoComObj::run()
{
    xVObj_Basics::run();
    // send and receive every time triggered

    // identify connection
    xSerialPortPtr pSerial=nullptr;

    // connect to data
    for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
    {
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
        {
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                // find all vis property objects
                xVArduinoConnectObj *pIObj = dynamic_cast<xVArduinoConnectObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("port"))
                    pSerial=(*pIObj->outputParamMap())["port"]._value.value<xSerialPortPtr>();
            }
        }
    }
    if (!pSerial || !pSerial->isOpen()) return;

    // init and start a send and receiver thread
}

void xVArduinoComObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-15,0);
    path.lineTo(60-25,0);
    path.arcTo(QRectF(80-25,-6.5,15,12.5),180,180);
    path.lineTo(105-25,0);
    path.lineTo(110-25,6.5);
    path.arcTo(QRectF(105-25,6.25,15,12.5),90,-180);
    path.lineTo(110-25,25-6.5);
    path.lineTo(105-25,25);

    path.lineTo(-15,25);
    path.lineTo(-10,25-6.5);
    path.arcTo(QRectF(-15,6.25,15,12.5),270,180);
    path.lineTo(-10,6.5);
    path.lineTo(-15,0);

    QPen pen(Qt::black);
    QRadialGradient rg(-15,0, 80, 30, 1, 5);
    rg.setColorAt(0,QColor(0,255,0));
    rg.setColorAt(0.4,QColor(255,255,255));
    rg.setColorAt(0.6,QColor(255,255,255));
    rg.setColorAt(1,QColor(255,0,0));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(105,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(-25,-15,145,45));
/*
    pBoundingRectItem = new QGraphicsRectItem(pGrpItem->boundingRect());
    pBoundingRectItem->setPen(QPen(Qt::white,2));
    pBoundingRectItem->setBrush(QBrush(Qt::NoBrush));
    pGrpItem->addToGroup(pBoundingRectItem);
*/
    pStatusItem->moveBy(5,0);
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
