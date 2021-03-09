#ifndef XVTRAFFICLIGHTOBJ_H
#define XVTRAFFICLIGHTOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"

class xVTrafficLightObj:public xVObj_Basics
{
    Q_OBJECT
public:
    xVTrafficLightObj(QString txt="");
    xVTrafficLightObj(QDataStream &d);
    virtual void save(QDataStream& d,bool _explicit=false) override;
    virtual void run() override;
    virtual void paramModified(const QString& txt="") override;
protected:
    void generateShape();
    QGraphicsPixmapItem *pTrafficLightPixItem = nullptr;
};

#endif // XVTRAFFICLIGHTOBJ_H
