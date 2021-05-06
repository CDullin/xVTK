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
    _iconMp["output"]="://images/view.png";
    _iconMp["tools"]="://images/tool.png";
    _iconMp["logic"]="://images/logic.png";
    _iconMp["arduino"]="://images/arduino.png";
    _iconMp["filter"]="://images/filter.png";
    _iconMp["input connector"]="://images/connection_symbol.png";
    _iconMp["parmeter output connector"]="://images/param_connection_symbol.png";
    _iconMp["report"]="://images/report.png";
    _iconMp["dashboard"]="://images/dashboard.png";
    _iconMp["session"]="://images/session.png";
}
