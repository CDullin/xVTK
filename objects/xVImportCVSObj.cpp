#include "xVImportCVSObj.h"

xVImportCVSObj::xVImportCVSObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_CVS;
    _paramMp["file name"]._id=1;
    _paramMp["file name"]._value=QVariant::fromValue(QFileInfo(::_settings["last cvs file"]._value.toString()));
    _paramMp["name in global namespace"]._id=2;
    _paramMp["name in global namespace"]._value="table1";
    _paramMp["separator"]._id=3;
    _paramMp["separator"]._value=QString(",");
    _paramMp["header skip [lines]"]._id=4;
    _paramMp["header skip [lines]"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,100));
    _paramMp["skip every N [line]"]._id=5;
    _paramMp["skip every N [line]"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,100));
    _paramMp["skip every N [column]"]._id=6;
    _paramMp["skip every N [column]"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,100));
    _paramMp["data ordered in columns"]._id=7;
    _paramMp["data ordered in columns"]._value=true;
    _paramMp["autodetect data type"]._id=8;
    _paramMp["autodetect data type"]._value=true;
    _paramMp["use first line as label"]._id=9;
    _paramMp["use first line as label"]._value=true;
    _paramMp["decimal point"]._id=10;
    _paramMp["decimal point"]._value=QString(".");
    _hasPreview = true;
}

xVImportCVSObj::xVImportCVSObj(QDataStream& d):xVGenImpObj(d)
{
    _type = xVOT_CVS;
    _hasPreview = true;
}

void xVImportCVSObj::run()
{
    if (status()==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);
        // import table into global namespace
        setStatus(OS_VALID);
    }
}

void xVImportCVSObj::reset()
{
    // remove data
    setStatus(OS_UPDATE_NEEDED);
}
