#ifndef XVREPORTOBJ_H
#define XVREPORTOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisObj.h"
#include "xVReport.h"

class xVReportObj:public xVGenVisObj
{
    Q_OBJECT
public:
    xVReportObj(const QString& txt="");
    xVReportObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;
public slots:
    virtual void KSlot(const SIG_TYPE&,void* data=nullptr) override;
protected slots:
    void paramModified(const QString& txt="") override;
protected:
    xVReport *pReportRef = nullptr;
};


#endif // XVREPORTOBJ_H
