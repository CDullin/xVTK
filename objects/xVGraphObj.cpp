#include "xVGraphObj.h"

xVGraphObj::xVGraphObj(const QString& txt):xVGenVisObj(txt)
{
    _type = xVOT_GRAPH;
    _description = "Used to display data as graphs";
}
xVGraphObj::xVGraphObj(QDataStream& d):xVGenVisObj(d)
{
    _type =xVOT_GRAPH;
}
void xVGraphObj::run()
{
    xVGenVisObj::run();
    if (_status==OS_UPDATE_NEEDED)
    {
        if (!pWdgt)
        {
            pWdgt = new xVGraphWdgt(nullptr);
        }
        emit KSignal(ST_ADD_VISUALIZATION_WDGT,pWdgt);
        setStatus(OS_VALID);
    }
}
void xVGraphObj::reset()
{
    if (pWdgt)
    {
        emit KSignal(ST_REMOVE_VISUALIZATION_WDGT,pWdgt);
        delete pWdgt;
        pWdgt=nullptr;
    }
}
void xVGraphObj::paramModified(const QString& txt)
{
    xVGenVisObj::paramModified(txt);
}
