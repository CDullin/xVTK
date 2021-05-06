#ifndef XVGENIMPOBJ_H
#define XVGENIMPOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVGenImpObj:public xVObj_Basics
{
Q_OBJECT
public:
    xVGenImpObj(const QString& txt="");
    xVGenImpObj(QDataStream &d);
    virtual void paramModified(const QString& txt) override;
    virtual void save(QDataStream& d,bool _explicit=false) override {
        xVObj_Basics::save(d,_explicit);};
    virtual void run() override;
protected:
    virtual void generateShape();
};

#endif // XVGENIMPOBJ_H
