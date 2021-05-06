#ifndef XVGRAPHWDGT_H
#define XVGRAPHWDGT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include "xVTypes.h"

class xVDataSeries
{
public:
    xParamMap* parameterMap(){return &_paramMp;}
    void drawSeries();
protected:
    QVector <QVector4D> _data;
    xParamMap _paramMp;
};

class xVAxis:public QGraphicsPathItem
{
public:
    xVAxis():QGraphicsPathItem(){}
    xParamMap* parameterMap(){return &_paramMp;}
    void drawAxis();
protected:
    xParamMap _paramMp;
};

class xVGraphWdgt:public QGraphicsView
{
    Q_OBJECT

public:
    xVGraphWdgt(QWidget*);
    void attachAxisToSeries(xVAxis*,xVDataSeries*);
    void addSeries(xVDataSeries*);
    void removeAxis(xVAxis*);
    void removeSeries(xVDataSeries*);
    QList<xVAxis*>* axes(){return &_axesLst;};
    QList<xVDataSeries*>* series(){return &_seriesLst;}
    xParamMap* parameterMap(){return &_paramMp;}
signals:
    void KSignal(const SIG_TYPE& type,void* pData=nullptr);
public slots:
    void KSlot(const SIG_TYPE& type,void* pData=nullptr);
protected:
    virtual void resizeEvent(QResizeEvent*) override;
    void updateGraph();

    xParamMap _paramMp;
    QList <xVDataSeries*> _seriesLst;
    QList <xVAxis*> _axesLst;

    QGraphicsPathItem *pBackgroundItem = nullptr;
};

#endif // XVGRAPHWDGT_H
