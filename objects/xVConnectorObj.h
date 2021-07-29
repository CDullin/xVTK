#ifndef XVCONNECTOROBJ_H
#define XVCONNECTOROBJ_H

#include "xVObjects.h"
#include "xVTypes.h"
#include "xVHistoNodeItem.h"

class xConnectorObj:public xVAbstractBaseObj
{
Q_OBJECT
public:

    struct SECTION
    {
    public:
        float _pos;
        bool _horizontal;
        SECTION(){_pos=0;_horizontal=true;}
        SECTION(const float& p,const bool& h){_pos=p;_horizontal=h;}
        SECTION(QDataStream &d)
        {
            d >> _pos >> _horizontal;
        }
        void save(QDataStream& d){ d << _pos << _horizontal;}
    };

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
    virtual void paramModified(const QString& txt="") override {
        QString calledWithTxt=txt;
        update();
    }
    virtual void save(QDataStream&, bool _explicit=false) override;

    void addNodeAtCursorPos();

public slots:
    void update();
signals:
    void parameterModified();

protected slots:
    void patternTimeOut();
    void updatePath();
    void nodeMoved(QPointF,const int&,xVHistoNodeItem*);

protected:
    void generatePattern();
    QGraphicsPathItem *generateArrow();
    void createPath();
    void addNode(QPointF p,int i,xVHistoNodeItem::HNI_MODE m);

    xConnector *pInObj=nullptr,*pOutObj=nullptr;
    xGroupItem *pGrpItem=nullptr;
    QGraphicsPathItem *pPathItem=nullptr;
    QGraphicsPathItem *pPathStartItem=nullptr;
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
    QList <SECTION> _sectionLst;

    QPointF oldIPnt,oldOPnt;
    bool _dontResetControlPoints = false;

    long _patternId=0;
    QTimer _patternTimer;
    QPen _linePen;
};

#endif // XVCONNECTOROBJ_H
