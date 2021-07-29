#ifndef XVMEASUREMENTREGION_H
#define XVMEASUREMENTREGION_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVMeasurementRegion:public xVObj_Basics
{
Q_OBJECT
public:
    xVMeasurementRegion(const QString&);
    xVMeasurementRegion(QDataStream& d);
    virtual void run() override;
    virtual void reset() override;
    virtual void save(QDataStream& d,bool _explicit=false) override;
protected:
    void generateShape();
public slots:
    virtual void paramModified(const QString& txt="") override;
};

#endif // XVMEASUREMENTREGION_H
