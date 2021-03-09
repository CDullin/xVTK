#ifndef XVCALLEXTERNALOBJ_H
#define XVCALLEXTERNALOBJ_H

#include "xVTypes.h"
#include "xVGenImpObj.h"

class xVCallExternalObj:public xVGenImpObj
{
Q_OBJECT
public:
    xVCallExternalObj(const QString& txt="");
    xVCallExternalObj(QDataStream &d);
    virtual void save(QDataStream& d,bool _explicit=false) override {xVObj_Basics::save(d,_explicit);};
    virtual void run() override;
protected:
    virtual void generateShape() override;
};




#endif // XVCALLEXTERNALOBJ_H
