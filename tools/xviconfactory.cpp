#include "xviconfactory.h"

xVIconFactory _iconFactory;

xVIconFactory::xVIconFactory()
{
    populateMap();
}

void xVIconFactory::populateMap()
{
    _iconMp.clear();
}
