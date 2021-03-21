#ifndef XVWAITOBJ_H
#define XVWAITOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVWaitObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVWaitObj(const QString&);
    xVWaitObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;
public slots:
    virtual void paramModified(const QString& txt="") override;
protected slots:
    void updateTimeDisplay();
protected:
    void generateShape();

    QGraphicsPathItem *pPointer;
    QGraphicsPathItem *pBack;
    QTimer tim;
    double _time;
    bool _totalTimeReached = false;
};


#endif // XVWAITOBJ_H
