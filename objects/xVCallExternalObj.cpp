#include "xVCallExternalObj.h"
#include <QDir>
#include <QProcess>

xVCallExternalObj::xVCallExternalObj(const QString& txt):xVGenImpObj()
{
    _type = xVOT_CALL_EXTERNAL;    
    _maxInput = 100;
    _maxOutput = 100;

    _paramMp["name"]._value = txt;
    _paramMp["name"]._id = 0;
    _paramMp["file name"]._value = QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"unknown"));
    _paramMp["file name"]._id = 1;
    _paramMp["arguments"]._value = "";
    _paramMp["arguments"]._id = 2;
    _paramMp["wait for finish"]._value = false;
    _paramMp["wait for finish"]._id = 3;
    _paramMp["run in folder"]._value = QVariant::fromValue(xFileName(xFileName::FN_INPUT_DIR,QDir::currentPath()));
    _paramMp["run in folder"]._id = 4;
    generateShape();

    xConnector* pParamInputCon=new xConnector(this);
    pParamInputCon->setToParamInput();
    pParamInputCon->item()->moveBy(45,-10);
    _connectorLst.append(pParamInputCon);
    xConnector* pInputCon = new xConnector(this);
    _connectorLst.append(pInputCon);
    pInputCon->setToInput();
    pInputCon->item()->moveBy(0,12.5);
    pGrpItem->addToGroup(pInputCon->item());
    xConnector* pOutputCon=new xConnector(this);
    _connectorLst.append(pOutputCon);
    pOutputCon->setToOutput();
    pOutputCon->item()->moveBy(105,12.5);
    pGrpItem->addToGroup(pOutputCon->item());

    pGrpItem->addToGroup(pParamInputCon->item());

    connect(pParamInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pInputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    connect(pOutputCon,SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
}

xVCallExternalObj::xVCallExternalObj(QDataStream& d):xVGenImpObj(d)
{
    _type = xVOT_CALL_EXTERNAL;
    _maxInput = 100;
    _maxOutput = 100;

    generateShape();

    for (int i=0;i<_connectorLst.count();++i)
    {
        pGrpItem->addToGroup(_connectorLst[i]->item());
        connect(_connectorLst[i],SIGNAL(activated(xConnector*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated_SLOT(xConnector*,xCONNECTOR_TYPE)));
    }
}

void xVCallExternalObj::run()
{
    xVGenImpObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;

    setStatus(OS_RUNNING);

    QFileInfo info = _paramMp["file name"]._value.value<xFileName>()._fileName;
    QFileInfo path = _paramMp["run in folder"]._value.value<xFileName>()._fileName;
    QStringList arguments = _paramMp["arguments"]._value.toString().split(",");
    bool _wait = _paramMp["wait for finish"]._value.toBool();

    if (info.isFile() && info.exists())
    {
        QProcess *myProcess=new QProcess(this);
        myProcess->setProgram(info.absoluteFilePath());
        myProcess->setArguments(arguments);
        myProcess->setWorkingDirectory(path.absoluteFilePath());
        myProcess->start();
        myProcess->state()==QProcess::FailedToStart ? setStatus(OS_ERROR) : setStatus(OS_VALID);
        if (_wait) myProcess->waitForFinished();
    }
    else
    {
        emit KSignal(ST_ERROR_MSG,new QString("program not found"));
        setStatus(OS_ERROR);
    }
}

void xVCallExternalObj::generateShape()
{
    QPainterPath path;
    path.moveTo(-10,0);
    path.lineTo(80,0);
    path.lineTo(70,25);
    path.lineTo(-10,25);
    path.lineTo(0,12.5);
    path.lineTo(-10,0);
    path.moveTo(85,0);
    path.arcTo(QRectF(65,0,40,25),90,-180);
    path.lineTo(75,25);
    path.lineTo(85,0);

    QPen pen(Qt::black);
    QRadialGradient rg(10,10, 100, 0, 1, 5);
    rg.setColorAt(0,QColor(200,200,200));
    rg.setColorAt(1,QColor(100,100,100));

    QBrush brush(rg);
    pShapeItem = new QGraphicsPathItem(path);
    pShapeItem->setPen(pen);
    pShapeItem->setBrush(brush);

    pTxtItem = new xScrollTxtItem(90,_paramMp["name"]._value.toString());
    pTxtItem->setPen(QPen(Qt::black));
    pTxtItem->setPos(5,5);

    pGrpItem = new xGroupItem();
    connect(pGrpItem,SIGNAL(placed()),this,SIGNAL(placed()));
    connect(pGrpItem,SIGNAL(moved()),this,SIGNAL(moved()));
    connect(pGrpItem,SIGNAL(selected()),this,SLOT(grpSelected()));
    pGrpItem->addToGroup(pShapeItem);
    pGrpItem->addToGroup(pTxtItem);
    pGrpItem->setBoundingRectSize(QRectF(-22,-15,152,45));
/*
    pBoundingRectItem = new QGraphicsRectItem(pGrpItem->boundingRect());
    pBoundingRectItem->setPen(QPen(Qt::white,2));
    pBoundingRectItem->setBrush(QBrush(Qt::NoBrush));
    pGrpItem->addToGroup(pBoundingRectItem);
*/
    QPen p2(Qt::black);
    p2.setWidth(2);


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
