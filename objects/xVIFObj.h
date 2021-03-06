#ifndef XVIFOBJ_H
#define XVIFOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVIFObj:public xVObj_Basics
{
Q_OBJECT
public:
    xVIFObj(const QString& txt="");
    xVIFObj(QDataStream &d);
    virtual void save(QDataStream& d,bool _explicit=false) override {xVObj_Basics::save(d,_explicit);};
    virtual void run() override;
    virtual void reset() override;
signals:
    void addVisWdgt(QWidget*);
protected:
    void generateShape();
    QGraphicsSimpleTextItem *pYesItem;
    QGraphicsSimpleTextItem *pNoItem;
    xConnector* pYesOutputCon;
    xConnector* pNoOutputCon;
};


#endif // XVIFOBJ_H
