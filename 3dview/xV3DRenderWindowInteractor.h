#ifndef XV3DRENDERWINDOWINTERACTOR_H
#define XV3DRENDERWINDOWINTERACTOR_H

#include <vtkRenderWindowInteractor.h>
#include <QObject>
#include "xVTypes.h"

class xV3DRenderWindowInteractor:public QObject,public vtkRenderWindowInteractor
{
Q_OBJECT
public:
    xV3DRenderWindowInteractor(QObject* parent);
public slots:
    virtual void KSlot(const SIG_TYPE&,void* data=nullptr);
signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
};

#endif // XV3DRENDERWINDOWINTERACTOR_H
