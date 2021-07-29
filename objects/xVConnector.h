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
    xVObj_Basics* baseObj();
    void setToInput();
    void setToOutput();
    void setToParamInput();
    void setEnabled(bool b);
    bool isEnabled();
    void addConObject(xVObj_Basics* pVObj);
    void removeConObj(xVObj_Basics* pVObj);
    bool connectedTo(xVObj_Basics* pVObj);
    QList <xVObj_Basics*>* connectedObjects();
    xCONNECTOR_TYPE type();
    QGraphicsItemGroup *item();
    void setActivated(bool b);
    bool isActivated();
    QPointF pos();
    void save(QDataStream& d);
    QString id();
    QStringList* objIDs();
protected slots:
    void connectorActivated();
    void updateToolTip();
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
