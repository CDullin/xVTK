#include "xV3DRenderWindowInteractor.h"

/*
- left mouse rotate x-y plane
- shift left mouse rotate z axis
- right mouse move x-y plane
- shift right mouse move z axis
- wheel zoom in out
- ctrl left mouse context menu

// keys
alt+a group
alt+q single
alt+s screen shot
alt+d defined rotation
alt+r reset camera

// conext menu
- list for grouping the actors + defined movement
- screen shot
- camera / defined view angles
*/

xV3DRenderWindowInteractor::xV3DRenderWindowInteractor(QObject *parent):QObject(parent),vtkRenderWindowInteractor()
{
    //vtkRenderWindowInteractor::New();
}

void xV3DRenderWindowInteractor::KSlot(const SIG_TYPE& type,void *pData)
{

}

