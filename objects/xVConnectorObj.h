#ifndef XVCONNECTOROBJ_H
#define XVCONNECTOROBJ_H

#include "xVObjects.h"
#include "xVTypes.h"
#include "xVHistoNodeItem.h"

class xConnectorObj:public xAbstractBasisObj
{
Q_OBJECT
public:
    xConnectorObj(QDataStream &d);
    xConnectorObj(xConnector* pIn,xConnector* pOut,bool _paramConnection=false);
    ~xConnectorObj();

    void setInputObj(xConnector* pIn){pInObj=pIn;}
    void setOutputObj(xConnector* pOut){pOutObj=pOut;}
    void setParameterConnection(const bool& b){_paramConnection=b;}
    void generateShape();
    QString id(){return _id;}

    QGraphicsItemGroup *item(){return pGrpItem;}
    xConnector* inputObj(){return pInObj;}
    xConnector* outputObj(){return pOutObj;}
    virtual void setParamSelected(bool b) override;
    virtual void paramModified(const QString& txt) override {
        update();
    }
    virtual void save(QDataStream&, bool _explicit=false) override;
public slots:
    void update();
signals:
    void parameterModified();

protected slots:
    void updatePath();

protected:
    QGraphicsPathItem *generateArrow();
    void createPath();

    xConnector *pInObj=nullptr,*pOutObj=nullptr;
    xGroupItem *pGrpItem=nullptr;
    QGraphicsPathItem *pPathItem=nullptr;
    bool _paramConnection = false;
    QString _id;
    QList <QGraphicsPathItem*> _arrowLst;

    QPropertyAnimation *paAnimation2;
    QGraphicsDropShadowEffect *pSelEffect;
    QTimer *pSelectionTimer;
    QGraphicsDropShadowEffect *pEffect;
    QGraphicsPathItem *pEndItem;
    QGraphicsEllipseItem *pEllipseItem;
    QGraphicsItemGroup *pEndGrp;

    QList <xVHistoNodeItem*> _nodeLst;
};

#endif // XVCONNECTOROBJ_H
