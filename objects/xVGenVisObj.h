#ifndef XVGENVISOBJ_H
#define XVGENVISOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVGenVisObj:public xVObj_Basics
{
Q_OBJECT
public:
    xVGenVisObj(const QString& txt="");
    xVGenVisObj(QDataStream &d);
    virtual void save(QDataStream& d,bool _explicit=false) override {xVObj_Basics::save(d,_explicit);};
    virtual void run() override;
protected:
    void generateShape();
};



#endif // XVGENVISOBJ_H
