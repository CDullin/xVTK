#include "xVArduinoConnectObj.h"
#include <QSerialPortInfo>

xVArduinoConnectObj::xVArduinoConnectObj(const QString& txt):xVObj_Basics()
{
    _type=xVOT_ARDUINO_CONNECT;
    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["serial port"]._id = 1;
    _paramMp["serial port"]._value=QString("/dev/ttyACM0");
    QList <QSerialPortInfo> available = QSerialPortInfo::availablePorts();
    QStringList ports;
    for (QList <QSerialPortInfo>::iterator it=available.begin();it!=available.end();++it)
        ports << it->portName();
    _optionLsts["serial port"]=ports;
    // enumerate all ports
    _paramMp["auto configuration"]._id = 2;
    _paramMp["auto configuration"]._value = "On";
    _optionLsts["auto configuration"] << "On" << "Off";
    _paramMp["BAUD rate"]._id = 10;
    _paramMp["BAUD rate"]._value = QVariant::fromValue(xLimitedInt(9600,10,10000));
    _paramMp["BAUD rate"]._subGrp = "Off";
    _paramMp["BREAK enabled"]._id = 11;
    _paramMp["BREAK enabled"]._value = true;
    _paramMp["BREAK enabled"]._subGrp = "Off";
    _paramMp["DATA bits"]._id = 12;
    _paramMp["DATA bits"]._value = "eight";
    _paramMp["DATA bits"]._subGrp = "Off";
    _optionLsts["DATA bits"] << "five" << "six" << "seven" << "eight";
    _paramMp["STOP bits"]._id = 13;
    _paramMp["STOP bits"]._value = "One";
    _paramMp["STOP bits"]._subGrp = "Off";
    _optionLsts["STOP bits"] << "One" << "0neAndHalf" << "Two";

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

    // change
    _outputParamMp["connected"]._id = 1;
    _outputParamMp["connected"]._value = false;
    _outputParamMp["port"]._id=2;
    _outputParamMp["port"]._value=QVariant::fromValue(xSerialPortPtr(0));
}
xVArduinoConnectObj::xVArduinoConnectObj(QDataStream& d):xVObj_Basics(d)
{
    _type=xVOT_ARDUINO_CONNECT;
    _maxInput=1000;
    generateShape();
    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }

    // change
    _outputParamMp["connected"]._id = 1;
    _outputParamMp["connected"]._value = false;
    _outputParamMp["port"]._id=2;
    _outputParamMp["port"]._value=QVariant::fromValue(xSerialPortPtr(0));
}
void xVArduinoConnectObj::reset()
{
    if (_outputParamMp["port"]._value.value<xSerialPortPtr>() && _outputParamMp["port"]._value.value<xSerialPortPtr>()->isOpen())
        _outputParamMp["port"]._value.value<xSerialPortPtr>()->close();
    setStatus(OS_UPDATE_NEEDED);
}
void xVArduinoConnectObj::run()
{
    xVObj_Basics::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);
        if (_outputParamMp["port"]._value.value<xSerialPortPtr>()==nullptr)
        {
            _outputParamMp["port"]._value = QVariant::fromValue(new QSerialPort(this));
        }
        _outputParamMp["port"]._value.value<xSerialPortPtr>()->setPortName(_paramMp["serial port"]._value.toString());
        if (!_paramMp["auto configuration"]._value.toBool())
        {
            // set all param manually

            _outputParamMp["port"]._value.value<xSerialPortPtr>()->setBaudRate(_paramMp["BAUD rate"]._value.value<xLimitedInt>()._value);
            _outputParamMp["port"]._value.value<xSerialPortPtr>()->setBreakEnabled(_paramMp["BREAK enabled"]._value.toBool());

            int id=_optionLsts["DATA bits"].indexOf(_paramMp["DATA bits"]._value.toString());
            int encoding[4]={QSerialPort::Data5,QSerialPort::Data6,QSerialPort::Data7,QSerialPort::Data8};
            _outputParamMp["port"]._value.value<xSerialPortPtr>()->setDataBits((QSerialPort::DataBits)encoding[id]);

            id=_optionLsts["STOP bits"].indexOf(_paramMp["STOP bits"]._value.toString());
            int encoding2[3]={QSerialPort::OneStop,QSerialPort::OneAndHalfStop,QSerialPort::TwoStop};
            if (id>=0 && id<=2)
                _outputParamMp["port"]._value.value<xSerialPortPtr>()->setStopBits((QSerialPort::StopBits)encoding2[id]);

        }
        if (_outputParamMp["connected"]._value.toBool()==false)
        {
            if (_outputParamMp["port"]._value.value<xSerialPortPtr>()->open(QIODevice::ReadWrite))
            {
                setStatus(OS_VALID);
                _outputParamMp["connected"]._value=true;
                if (_paramMp["auto configuration"]._value.toBool())
                {
                    // retrieve all parameter

                    _paramMp["BAUD rate"]._value = QVariant::fromValue(xLimitedInt(_outputParamMp["port"]._value.value<xSerialPortPtr>()->baudRate(),10,10000));
                    _paramMp["BREAK enabled"]._value = _outputParamMp["port"]._value.value<xSerialPortPtr>()->isBreakEnabled();
                    _paramMp["DATA bits"]._value = QString("%1").arg(_outputParamMp["port"]._value.value<xSerialPortPtr>()->dataBits());
                    QStringList encoding; encoding << "invalid" << "One" << "Two" << "OneAndHalf";
                    _paramMp["STOP bits"]._value = encoding[_outputParamMp["port"]._value.value<xSerialPortPtr>()->stopBits()];
                }
            }
            else
            {
                emit KSignal(ST_ERROR_MSG,new QString(QString("Couldn't open serial port connection to port %1").arg(_paramMp["serial port"]._value.toString())));
                _outputParamMp["connected"]._value=false;
                setStatus(OS_ERROR);
            }
        }
    }
}
void xVArduinoConnectObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-15,0);
    path.lineTo(105-25,0);
    path.lineTo(110-25,6.5);
    path.arcTo(QRectF(105-25,6.25,15,12.5),90,-180);
    path.lineTo(110-25,25-6.5);
    path.lineTo(105-25,25);
    path.arcTo(QRectF(55,25-6.5,15,12.5),0,-180);

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

    pTxtItem = new xScrollTxtItem(130,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(-22,-15,140,47));
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
