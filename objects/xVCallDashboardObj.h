#ifndef XVCALLDASHBOARDOBJ_H
#define XVCALLDASHBOARDOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVCallDashboardObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVCallDashboardObj(const QString& txt);
    xVCallDashboardObj(QDataStream& d);
    virtual void run() override;

protected slots:
    virtual void paramModified(const QString& txt="") override;

protected:
    void generateShape();
    QGraphicsPixmapItem *pPixItem = nullptr;
};

#endif // XVCALLDASHBOARDOBJ_H
