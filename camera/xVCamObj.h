#ifndef XVCAMOBJ_H
#define XVCAMOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenImpObj.h"

#include <pylon/InstantCamera.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

// Namespace for using pylon objects.
using namespace Pylon;

//!
//! \brief The xVCamObj class
//! connects to a pylon camera device and provides a instant camera pointer and device information
//!
class xVCamObj:public xVGenImpObj
{
    Q_OBJECT
public:
    xVCamObj(const QString& txt);
    xVCamObj(QDataStream &);
    virtual void run() override;
    virtual void reset() override;
protected:
    CInstantCamera *pCamera = NULL;
};

#endif // XVCAMOBJ_H
