#include "xVTextPropertyDlgItem.h"
#include "xVFontDefinitionDlg.h"
#include <QFontDatabase>

xVTextProp::xVTextProp(){
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
xVTextProp::xVTextProp(const xVTextProp& other){
    // copy map
    _paramMp = other._paramMp;
    // deep copy prop
    generateVtkTextPropFromParam();
}
void xVTextProp::generateParamFromVtkTextProp(vtkTextProperty *pProp)
{
    pVtkTextProp=pProp;
    _paramMp["font family"]._value=QString(pVtkTextProp->GetFontFamilyAsString());
    double col[3];pVtkTextProp->GetColor(col);
    QColor color;color.setRedF(col[0]);color.setGreenF(col[1]);color.setBlueF(col[2]);
    _paramMp["color"]._value=QVariant::fromValue(color);
    _paramMp["opacity"]._value=QVariant::fromValue(xLimitedDouble(pVtkTextProp->GetOpacity(),0,1,10));
    pVtkTextProp->GetBackgroundColor(col);
    color.setRedF(col[0]);color.setGreenF(col[1]);color.setBlueF(col[2]);
    _paramMp["background color"]._value=QVariant::fromValue(color);
    _paramMp["background opacity"]._value=QVariant::fromValue(xLimitedDouble(pVtkTextProp->GetBackgroundOpacity(),0,1,10));
    pVtkTextProp->GetFrameColor(col);
    color.setRedF(col[0]);color.setGreenF(col[1]);color.setBlueF(col[2]);
    _paramMp["frame color"]._value=QVariant::fromValue(color);
    _paramMp["frame visible"]._value=(bool)pVtkTextProp->GetFrame();
    _paramMp["frame width"]._value=QVariant::fromValue(xLimitedInt(pVtkTextProp->GetFrameWidth(),0,100));
    _paramMp["size"]._value=QVariant::fromValue(xLimitedInt(pVtkTextProp->GetFontSize(),0,100));
    _paramMp["bold"]._value=(bool)pVtkTextProp->GetBold();
    _paramMp["italic"]._value=(bool)pVtkTextProp->GetItalic();
    _paramMp["shadow"]._value=(bool)pVtkTextProp->GetShadow();
    pVtkTextProp->GetShadowColor(col);
    color.setRedF(col[0]);color.setGreenF(col[1]);color.setBlueF(col[2]);
    _paramMp["shadow color"]._value=QVariant::fromValue(color);
    int shadow_offset[2];pVtkTextProp->GetShadowOffset(shadow_offset);
    _paramMp["shadow offset X [px]"]._value=QVariant::fromValue(xLimitedInt(shadow_offset[0],-100,100));
    _paramMp["shadow offset Y [px]"]._value=QVariant::fromValue(xLimitedInt(shadow_offset[1],-100,100));
    switch (pVtkTextProp->GetJustification())
    {
    case VTK_TEXT_LEFT: _paramMp["horizontal alignment"]._value="left";break;
    case VTK_TEXT_CENTERED: _paramMp["horizontal alignment"]._value="center";break;
    case VTK_TEXT_RIGHT: _paramMp["horizontal alignment"]._value="right";break;
    }
    switch (pVtkTextProp->GetVerticalJustification())
    {
    case VTK_TEXT_TOP: _paramMp["vertical alignment"]._value="top";break;
    case VTK_TEXT_CENTERED: _paramMp["vertical alignment"]._value="center";break;
    case VTK_TEXT_BOTTOM: _paramMp["vertical alignment"]._value="bottom";break;
    }
    _paramMp["tight bounding box"]._value=(bool)pVtkTextProp->GetUseTightBoundingBox();
    _paramMp["orientation [°]"]._value=QVariant::fromValue(xLimitedInt(pVtkTextProp->GetOrientation(),0,360));
    _paramMp["line spacing [px]"]._value=QVariant::fromValue(xLimitedInt(pVtkTextProp->GetLineSpacing(),0,20));
    _paramMp["line offset [px]"]._value=QVariant::fromValue(xLimitedInt(pVtkTextProp->GetLineOffset(),0,20));
    //_paramMp["cell offset [px]"]._value=QVariant::fromValue(xLimitedInt(pVtkTextProp->GetCellOffset(),0,20));
}
vtkTextProperty* xVTextProp::generateVtkTextPropFromParam(){
    // generate or update vtkPointer
    if (!pVtkTextProp) pVtkTextProp=vtkTextProperty::New();
    QColor col = _paramMp["background color"]._value.value<QColor>();
    pVtkTextProp->SetBackgroundColor(col.redF(),col.greenF(),col.blueF());
    pVtkTextProp->SetBackgroundOpacity(_paramMp["background opacity"]._value.value<xLimitedDouble>()._value);
    pVtkTextProp->SetBold(_paramMp["bold"]._value.toBool());
    col = _paramMp["color"]._value.value<QColor>();
    pVtkTextProp->SetColor(col.redF(),col.greenF(),col.blueF());
    pVtkTextProp->SetFontFamilyAsString(_paramMp["font family"]._value.toString().toLatin1());
    pVtkTextProp->SetItalic(_paramMp["italic"]._value.toBool());
    pVtkTextProp->SetFontSize(_paramMp["size"]._value.value<xLimitedInt>()._value);
    pVtkTextProp->SetFrame(_paramMp["frame visible"]._value.toBool());
    col = _paramMp["frame color"]._value.value<QColor>();
    pVtkTextProp->SetFrameColor(col.redF(),col.greenF(),col.blueF());
    pVtkTextProp->SetOrientation(_paramMp["orientation [°]"]._value.value<xLimitedInt>()._value);
    pVtkTextProp->SetFrameWidth(_paramMp["frame width"]._value.value<xLimitedInt>()._value);
    switch (_optionLsts["horizontal alignment"].indexOf(_paramMp["horizontal alignment"]._value.toString()))
    {
    case 0: pVtkTextProp->SetJustification(VTK_TEXT_LEFT);break;
    case 1: pVtkTextProp->SetJustification(VTK_TEXT_CENTERED);break;
    case 2: pVtkTextProp->SetJustification(VTK_TEXT_RIGHT);break;
    }
    pVtkTextProp->SetLineOffset(_paramMp["line offset [px]"]._value.value<xLimitedInt>()._value);
    pVtkTextProp->SetOpacity(_paramMp["opacity"]._value.value<xLimitedDouble>()._value);
    pVtkTextProp->SetShadow(_paramMp["shadow"]._value.toBool());
    pVtkTextProp->SetShadowOffset(_paramMp["shadow offset X [px]"]._value.value<xLimitedInt>()._value,
                                    _paramMp["shadow offset Y [px]"]._value.value<xLimitedInt>()._value);
    pVtkTextProp->SetLineSpacing(_paramMp["line spacing [px]"]._value.value<xLimitedInt>()._value);
    pVtkTextProp->SetUseTightBoundingBox(_paramMp["tight bounding box"]._value.toBool());
    switch (_optionLsts["vertical alignment"].indexOf(_paramMp["vertical alignment"]._value.toString()))
    {
    case 0 : pVtkTextProp->SetVerticalJustification(VTK_TEXT_TOP);break;
    case 1 : pVtkTextProp->SetVerticalJustification(VTK_TEXT_CENTERED);break;
    case 2 : pVtkTextProp->SetVerticalJustification(VTK_TEXT_BOTTOM);break;
    }
    return pVtkTextProp;
}
QFont xVTextProp::generateQtFontFromParam()
{
    QFont f;
    f.setFamily(_paramMp["font family"]._value.toString());
    f.setBold(_paramMp["bold"]._value.toBool());
    f.setItalic(_paramMp["italic"]._value.toBool());
    f.setPixelSize(_paramMp["size"]._value.value<xLimitedInt>()._value);
    return f;
}
xVTextProp::~xVTextProp(){
    // free pointer
    if (pVtkTextProp) pVtkTextProp->Delete();pVtkTextProp=nullptr;
}
QDataStream &operator<<(QDataStream &out, const xVTextPropPtr &myObj){
    out << myObj->_paramMp;
    // save all to stream
    return out;}
QDataStream &operator>>(QDataStream &in, xVTextPropPtr &myObj){
    // generate pointer
    myObj = new xVTextProp();
    in >> myObj->_paramMp;
    myObj->generateVtkTextPropFromParam();
    // read all from stream
    return in;}
xVTextPropertyDlgItem::xVTextPropertyDlgItem(QWidget* parent):QToolButton(parent)
{
    setText("define font");
    connect(this,SIGNAL(clicked()),this,SLOT(clicked_SLOT()));
}
void xVTextPropertyDlgItem::setRef(xVTextProp* pR)
{
    pRefObj=pR;
}
void xVTextPropertyDlgItem::clicked_SLOT()
{
    if (!pRefObj) return;
    // start dialog
    xVFontDefinitionDlg dlg;
    dlg.setParameterRef(&(pRefObj->_paramMp));
    connect(&dlg,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SIGNAL(KSignal(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),&dlg,SLOT(KSlot(const SIG_TYPE&,void *)));
    dlg.exec();
    pRefObj->_paramMp=*(dlg.result());
    pRefObj->generateVtkTextPropFromParam();
    emit modified();
}
