#ifndef XVGENVISPROPOBJ_H
#define XVGENVISPROPOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVGenVisPropObj:public xVObj_Basics
{
Q_OBJECT
public:
    xVGenVisPropObj(const QString& txt="");
    xVGenVisPropObj(QDataStream &d);
    virtual void save(QDataStream& d,bool _explicit=false){xVObj_Basics::save(d,_explicit);};
protected:
    void generateShape();
};


#endif // XVGENVISPROPOBJ_H
