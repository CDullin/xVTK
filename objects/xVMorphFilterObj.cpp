#include "xVMorphFilterObj.h"

xVMorphFilterObj::xVMorphFilterObj(const QString& txt):xVGenFilterObj(txt)
{
    _type = xVOT_MORPH_FILTER;
    _description="Performs typical morphological filters";
}

xVMorphFilterObj::xVMorphFilterObj(QDataStream& d):xVGenFilterObj(d)
{
    _type = xVOT_MORPH_FILTER;
}

void xVMorphFilterObj::run()
{
    xVGenFilterObj::run();
    if (_status == OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);

        setStatus(OS_VALID);
    }
}
