#ifndef XVGENUSERDLGOBJ_H
#define XVGENUSERDLGOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVGenUserDlgObj:public xVObj_Basics
{
Q_OBJECT
public:
    xVGenUserDlgObj(const QString& txt="");
    xVGenUserDlgObj(QDataStream& d);
    virtual void save(QDataStream& d,bool _explicit=false) override {xVObj_Basics::save(d,_explicit);};
    virtual void run() override;
protected:
    void generateShape();
};

#endif // XVGENUSERDLGOBJ_H
