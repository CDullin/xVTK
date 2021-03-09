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
    virtual void save(QDataStream& d,bool _explicit=false){xVObj_Basics::save(d,_explicit);};
protected:
    void generateShape();
};



#endif // XVGENVISOBJ_H
