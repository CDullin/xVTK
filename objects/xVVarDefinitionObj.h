#ifndef XVVARDEFINITIONOBJ_H
#define XVVARDEFINITIONOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVVarDefinitionObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVVarDefinitionObj(QString txt="");
    xVVarDefinitionObj(QDataStream& d);
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
protected:
    virtual void generateShape();
};

#endif // XVVARDEFINITIONOBJ_H
