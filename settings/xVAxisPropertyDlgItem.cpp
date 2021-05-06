#include "xVAxisPropertyDlgItem.h"

#include "xVTextPropertyDlgItem.h"
#include "xVAxisDefinitionDlg.h"
#include <QFontDatabase>

xVAxisProp::xVAxisProp(){
    // init map
    QFontDatabase db;
    QFont f;
    _paramMp["font family"]._id=1;
    _optionLsts["font family"]=db.families();
    _paramMp["font family"]._value=f.family();
    _paramMp["color"]._id=2;
    _paramMp["color"]._value=QVariant::fromValue(QColor(255,255,255,255));
    _paramMp["opacity"]._id=3;
    _paramMp["opacity"]._value=QVariant::fromValue(xLimitedDouble(1,0,1,10));
    _paramMp["background color"]._id=4;
    _paramMp["background color"]._value=QVariant::fromValue(QColor(255,0,0,255));
    _paramMp["background opacity"]._id=5;
    _paramMp["background opacity"]._value=QVariant::fromValue(xLimitedDouble(0,0,1,10));
    _paramMp["frame color"]._id=6;
    _paramMp["frame color"]._value=QVariant::fromValue(QColor(0,255,255,255));
    _paramMp["frame visible"]._id=7;
    _paramMp["frame visible"]._value=false;
    _paramMp["frame width"]._id=8;
    _paramMp["frame width"]._value=QVariant::fromValue(xLimitedInt(2,0,100));
    _paramMp["size"]._id=9;
    _paramMp["size"]._value=QVariant::fromValue(xLimitedInt(20,0,100));
    _paramMp["bold"]._id=10;
    _paramMp["bold"]._value=false;
    _paramMp["italic"]._id=11;
    _paramMp["italic"]._value=false;
    _paramMp["shadow"]._id=12;
    _paramMp["shadow"]._value=false;
    _paramMp["shadow color"]._id=13;
    _paramMp["shadow color"]._value=QVariant::fromValue(QColor(0,0,0,255));
    _paramMp["shadow offset X [px]"]._id=14;
    _paramMp["shadow offset X [px]"]._value=QVariant::fromValue(xLimitedInt(1,-100,100));
    _paramMp["shadow offset Y [px]"]._id=15;
    _paramMp["shadow offset Y [px]"]._value=QVariant::fromValue(xLimitedInt(1,-100,100));
    _paramMp["horizontal alignment"]._id=16;
    _optionLsts["horizontal alignment"]=QStringList() << "left" << "center" << "right";
    _paramMp["horizontal alignment"]._value="center";
    _paramMp["vertical alignment"]._id=17;
    _optionLsts["vertical alignment"]=QStringList() << "top" << "center" << "bottom";
    _paramMp["vertical alignment"]._value="center";
    _paramMp["tight bounding box"]._id=18;
    _paramMp["tight bounding box"]._value=false;
    _paramMp["orientation [°]"]._id=19;
    _paramMp["orientation [°]"]._value=QVariant::fromValue(xLimitedInt(0,0,360));
    _paramMp["line spacing [px]"]._id=20;
    _paramMp["line spacing [px]"]._value=QVariant::fromValue(xLimitedInt(1,0,20));
    _paramMp["line offset [px]"]._id=21;
    _paramMp["line offset [px]"]._value=QVariant::fromValue(xLimitedInt(0,0,20));
    _paramMp["cell offset [px]"]._id=22;
    _paramMp["cell offset [px]"]._value=QVariant::fromValue(xLimitedInt(0,0,20));
}
xVAxisProp::xVAxisProp(const xVAxisProp& other){
    // copy map
    _paramMp = other._paramMp;
    // deep copy prop
    generateVtkAxisPropFromParam();
}
void xVAxisProp::generateParamFromVtkAxisProp(vtkAxisActor2D *pProp)
{
    pVtkAxisProp=pProp;
}
vtkAxisActor2D* xVAxisProp::generateVtkAxisPropFromParam(){
    // generate or update vtkPointer
    if (!pVtkAxisProp) pVtkAxisProp=vtkAxisActor2D::New();

    return pVtkAxisProp;
}
xVAxisProp::~xVAxisProp(){
    // free pointer
    if (pVtkAxisProp) pVtkAxisProp->Delete();pVtkAxisProp=nullptr;
}
QDataStream &operator<<(QDataStream &out, const xVAxisPropPtr &myObj){
    out << myObj->_paramMp;
    // save all to stream
    return out;}
QDataStream &operator>>(QDataStream &in, xVAxisPropPtr &myObj){
    // generate pointer
    myObj = new xVAxisProp();
    in >> myObj->_paramMp;
    myObj->generateVtkAxisPropFromParam();
    // read all from stream
    return in;}
xVAxisPropertyDlgItem::xVAxisPropertyDlgItem(QWidget* parent):QToolButton(parent)
{
    setText("define axis");
    connect(this,SIGNAL(clicked()),this,SLOT(clicked_SLOT()));
}
void xVAxisPropertyDlgItem::setRef(xVAxisProp* pR)
{
    pRefObj=pR;
}
void xVAxisPropertyDlgItem::clicked_SLOT()
{
    if (!pRefObj) return;
    // start dialog
    xVAxisDefinitionDlg dlg;
    dlg.setParameterRef(&(pRefObj->_paramMp));
    connect(&dlg,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SIGNAL(KSignal(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),&dlg,SLOT(KSlot(const SIG_TYPE&,void *)));
    dlg.exec();
    pRefObj->_paramMp=*(dlg.result());
    pRefObj->generateVtkAxisPropFromParam();
    emit modified();
}
