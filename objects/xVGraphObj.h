#ifndef XVGRAPHOBJ_H
#define XVGRAPHOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisObj.h"
#include "xVGraphWdgt.h"

class xVGraphObj:public xVGenVisObj
{
    Q_OBJECT
public:
    xVGraphObj(const QString& txt="");
    xVGraphObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;
protected slots:
    virtual void paramModified(const QString& txt="") override;
protected:
    xVGraphWdgt *pWdgt=nullptr;
    xParamMap _param;
    QList <xVDataSeries*> _seriesLst;
};

#endif // XVGRAPHOBJ_H
