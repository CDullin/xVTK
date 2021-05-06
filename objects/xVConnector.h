#ifndef XVCONNECTOR_H
#define XVCONNECTOR_H

#include "xVCustomGraphicItems.h"
#include <QDateTime>
#include "xVTypes.h"

class xVObj_Basics;

enum xCONNECTOR_TYPE
{
    xCT_INVALID,
    xCT_INPUT,
    xCT_OUTPUT,
    xCT_PARAMETER
};

class xConnector:public QObject
{
Q_OBJECT
public:
    xConnector(xVObj_Basics*,QDataStream &d);
    xConnector(xVObj_Basics* o);
    xVObj_Basics* baseObj(){return pBaseObj;}
    void setToInput();
    void setToOutput();
    void setToParamInput();
    void setEnabled(bool b);
    bool isEnabled(){return _enabled;}
    void addConObject(xVObj_Basics* pVObj);
    void removeConObj(xVObj_Basics* pVObj);
    bool connectedTo(xVObj_Basics* pVObj);
    QList <xVObj_Basics*>* connectedObjects(){return &_connectToLst;}
    xCONNECTOR_TYPE type(){return _type;}
    QGraphicsItemGroup *item(){return pGrpItem;}
    void setActivated(bool b){pEllipseItem->setActivated(b);}
    bool isActivated(){return pEllipseItem->isActivated();}
    QPointF pos(){
        QPointF p(15,0);
        switch (_type)
        {
        case xCT_INPUT: p=QPointF(-15,0);break;
        case xCT_OUTPUT: p=QPointF(15,0);break;
        case xCT_PARAMETER: p=QPointF(0,0);break;
        default:
            break;
        }
        return pEllipseItem->mapToScene(p);}
    void save(QDataStream& d);
    QString id(){return _id;}
    QStringList* objIDs(){return &_idLst;}
protected slots:
    void connectorActivated();
signals:
    void activated(xConnector*,xCONNECTOR_TYPE);
    void KSignal(const SIG_TYPE&,void* data=nullptr);
protected:
    void generateShape();
    xVObj_Basics* pBaseObj=nullptr;
    QList <xVObj_Basics*> _connectToLst;
    xCONNECTOR_TYPE _type;
    xEllipseItem *pEllipseItem = nullptr;
    QGraphicsLineItem *pLineItem = nullptr;
    QGraphicsItemGroup *pGrpItem = nullptr;
    QGraphicsPathItem *pEnabledItem = nullptr;
    bool _enabled = true;
    QStringList _idLst;
    QString _id;
};

#endif // XVCONNECTOR_H
