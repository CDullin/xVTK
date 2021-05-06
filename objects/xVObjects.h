#ifndef XVOBJECTS_H
#define XVOBJECTS_H

#include <QGraphicsPathItem>
#include <QPen>
#include "xVCustomGraphicItems.h"
#include "xVConnector.h"
#include "xVTypes.h"
#include "xVAbstractBaseObj.h"
#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QSoundEffect>

enum OBJ_STATUS
{
    OS_RUNNING,
    OS_VALID,
    OS_WARNING,
    OS_ERROR,
    OS_UPDATE_NEEDED
};

//!
//! \brief The xVObj_Basics class
//! is the base clase for all objects in the dashboard
//!
class xVObj_Basics:public xVAbstractBaseObj
{
Q_OBJECT
public:
    xVObj_Basics();
    xVObj_Basics(QDataStream &);
    ~xVObj_Basics();
    void setName(const QString& txt){_paramMp["name"]._value=txt;}
    QGraphicsItemGroup *item(){return pGrpItem;}
    xConnector* inConnector(xVObj_Basics* pVObj){return connectedToVObjWithType(pVObj,xCT_INPUT);};
    xConnector* paramInConnector(xVObj_Basics* pVObj){return connectedToVObjWithType(pVObj,xCT_PARAMETER);};
    xConnector* outConnector(xVObj_Basics* pVObj){return connectedToVObjWithType(pVObj,xCT_OUTPUT);};
    xConnector* activeConnector();
    QList <xConnector*>* connectorLst(){return &_connectorLst;}
    QMap<QString,xPROP_TYPE>* outputParamMap(){return &_outputParamMp;}
    int maxInput(){return _maxInput;}
    int maxOutput(){return _maxOutput;}
    int countOfInputs(){return countOf(xCT_INPUT);}
    int countOfOutputs(){return countOf(xCT_OUTPUT);}
    int countOfParamInputs(){return countOf(xCT_PARAMETER);}
    virtual void setParamSelected(bool b) override;
    OBJ_STATUS status(){return _status;}
    void setStatus(const OBJ_STATUS& s);
    virtual void run();
    void positionChanged(){emit moved();}
    virtual void save(QDataStream&,bool _explicit=false) override;
    virtual void reset(){setStatus(OS_UPDATE_NEEDED);};
public slots:
    virtual void paramModified(const QString& txt="") override;
signals:
    void moved();
    void placed();
    void connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE);
    void parameterModified();
protected slots:
    void connectorActivated_SLOT(xConnector* pCon,xCONNECTOR_TYPE type)
    {
        emit connectorActivated(this,type);
    }
    virtual void updateTimerTimeOut(){};
protected:

    xConnector* connectedToVObjWithType(xVObj_Basics*,const xCONNECTOR_TYPE&);
    int countOf(xCONNECTOR_TYPE);

    QList <xConnector*> _connectorLst;
    QGraphicsPathItem *pShapeItem=nullptr;
    xGroupItem *pGrpItem=nullptr;
    xScrollTxtItem *pTxtItem=nullptr;
    QGraphicsEllipseItem *pStatusItem = nullptr;
    QGraphicsPixmapItem* pIconItem = nullptr;
    QGraphicsRectItem* pBoundingRectItem = nullptr;
    QSoundEffect *pStartSoundEffect = nullptr;
    int _maxInput = 1;
    int _maxOutput = 100;
    //QList <xVObj_Basics*> _connectedOutputs;
    //QList <xVObj_Basics*> _connectedInputs;
    //QList <xVObj_Basics*> _connectedParamInputs;
    OBJ_STATUS _status=OS_UPDATE_NEEDED;
    QGraphicsColorizeEffect *pColEffect;    
    QPropertyAnimation *paAnimation;
    QTimer *pAnimationTimer;
    QPropertyAnimation *paAnimation2;
    QGraphicsDropShadowEffect *pSelEffect;
    QGraphicsDropShadowEffect *pEffect=nullptr;
    QTimer *pSelectionTimer;
    QMap<QString,xPROP_TYPE> _outputParamMp;
    QTimer* pUpdateTimer=nullptr;
};

#endif // XVOBJECTS_H
