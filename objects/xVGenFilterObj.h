#ifndef XVGENFILTEROBJ_H
#define XVGENFILTEROBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVGenFilterObj:public xVObj_Basics
{
Q_OBJECT
public:
    xVGenFilterObj(const QString& txt="");
    xVGenFilterObj(QDataStream &d);
    virtual void save(QDataStream& d,bool _explicit=false) override {xVObj_Basics::save(d,_explicit);};
    virtual void run() override;
protected:
    void generateShape();
};


#endif // XVGENFILTEROBJ_H
