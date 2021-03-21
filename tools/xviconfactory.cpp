#include "xviconfactory.h"
#include <QPixmap>

xVIconFactory _iconFactory;

xVIconFactory::xVIconFactory()
{
    populateMap();
}

void xVIconFactory::populateMap()
{
    _iconMp.clear();
    _iconMp["import"]="://images/import.png";
    _iconMp["user interaction"]="://images/user.png";
    _iconMp["properties"]="://images/properties.png";
    _iconMp["views"]="://images/view.png";
    _iconMp["tools"]="://images/tool.png";
    _iconMp["logic"]="://images/logic.png";
    _iconMp["arduino"]="://images/arduino.png";
    _iconMp["filter"]="://images/filter.png";
}
