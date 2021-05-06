#ifndef XVFUZZYRULEBASEOBJ_H
#define XVFUZZYRULEBASEOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVFuzzyRuleBaseObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVFuzzyRuleBaseObj(const QString&);
    xVFuzzyRuleBaseObj(QDataStream &d);
    virtual void run() override;

protected:
    void paramModified(const QString&) override;
    void generateShape();
};

#endif // XVFUZZYRULEBASEOBJ_H
