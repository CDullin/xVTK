#include "xVImageStackObj.h"

xVImageStackObj::xVImageStackObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_IMAGE_STACK;
    _paramMp["stack source type"]._id=1;
    _paramMp["stack source type"]._value="folder";
    _optionLsts["stack source type"] << "folder" << "volume file";
    _paramMp["folder name"]._id=2;
    _paramMp["folder name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_DIR,""));
    _paramMp["folder name"]._subGrp="folder";
    _paramMp["name filter"]._id=3;
    _paramMp["name filter"]._value="*";
    _paramMp["name filter"]._subGrp="folder";
    _paramMp["file name"]._id=4;
    _paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,""));
    _paramMp["file name"]._subGrp="volume file";
    _paramMp["virtual stack"]._id=5;
    _paramMp["virtual stack"]._value=true;
}

xVImageStackObj::xVImageStackObj(QDataStream &d):xVGenImpObj(d)
{
    _type = xVOT_IMAGE_STACK;

}
void xVImageStackObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        // load all or load as virtual stack
        setStatus(OS_VALID);
    }
}
void xVImageStackObj::reset()
{
    // unload image data
    setStatus(OS_UPDATE_NEEDED);
}
