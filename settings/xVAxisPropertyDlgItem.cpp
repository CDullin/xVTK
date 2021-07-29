#include "xVAxisPropertyDlgItem.h"
#include "xVAxisDefinitionDlg.h"
#include <QFontDatabase>

xVAxisProp::xVAxisProp(){
    // init map
    _paramMp["visible"]._id=1;
    _paramMp["visible"]._value=true;
    _paramMp["range"]._id=2;
    _paramMp["range"]._value=QVariant::fromValue(QPointF(0,1));
    _paramMp["ruler mode"]._id=5;
    _paramMp["ruler mode"]._value=true;
    _paramMp["dragable"]._id=6;
    _paramMp["dragable"]._value=false;
    _paramMp["pickable"]._id=7;
    _paramMp["pickable"]._value=false;
    _paramMp["height"]._id=8;
    _paramMp["height"]._value=QVariant::fromValue(xLimitedDouble(5,1,100,10));
    _paramMp["width"]._id=9;
    _paramMp["width"]._value=QVariant::fromValue(xLimitedDouble(5,1,100,10));
    _paramMp["layer"]._id=10;
    _paramMp["layer"]._value=QVariant::fromValue(xLimitedInt(1,1,100));
    _paramMp["font factor"]._id=11;
    _paramMp["font factor"]._value=QVariant::fromValue(xLimitedDouble(1,1,100,10));
    _paramMp["label visible"]._id=12;
    _paramMp["label visible"]._value=true;
    _paramMp["label factor"]._id=14;
    _paramMp["label factor"]._value=QVariant::fromValue(xLimitedDouble(1,1,100,10));
    _paramMp["label format"]._id=15;
    _paramMp["label format"]._value="";
    _paramMp["label font"]._id=16;
    _paramMp["label font"]._value=QVariant::fromValue((xVTextPropPtr)new xVTextProp());
    _paramMp["label number"]._id=17;
    _paramMp["label number"]._value=QVariant::fromValue(xLimitedInt(1,1,100));
    _paramMp["nice numbers"]._id=18;
    _paramMp["nice numbers"]._value=true;
    _paramMp["title"]._id=19;
    _paramMp["title"]._value="";
    _paramMp["title visible"]._id=20;
    _paramMp["title visible"]._value=true;
    _paramMp["title font"]._id=21;
    _paramMp["title font"]._value=QVariant::fromValue((xVTextPropPtr)new xVTextProp());
    _paramMp["title position"]._id=22;
    _paramMp["title position"]._value=QVariant::fromValue(xLimitedDouble(5,1,100,10));
    _paramMp["ticks visible"]._id=23;
    _paramMp["ticks visible"]._value=true;
    _paramMp["minor ticks"]._id=24;
    _paramMp["minor ticks"]._value=QVariant::fromValue(xLimitedInt(1,1,100));
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
    _paramMp["visible"]._value=(bool)pProp->GetVisibility();
    double range[2];pProp->GetRange(range);
    _paramMp["range"]._value=QVariant::fromValue(QPointF(range[0],range[1]));
    _paramMp["ruler mode"]._value=(bool)pProp->GetRulerMode();
    _paramMp["dragable"]._value=(bool)pProp->GetDragable();
    _paramMp["pickable"]._value=(bool)pProp->GetPickable();
    _paramMp["height"]._value=QVariant::fromValue(xLimitedDouble(pProp->GetHeight(),1,100,10));
    _paramMp["width"]._value=QVariant::fromValue(xLimitedDouble(pProp->GetWidth(),1,100,10));
    _paramMp["layer"]._value=QVariant::fromValue(xLimitedInt(pProp->GetLayerNumber(),1,100));
    _paramMp["font factor"]._value=QVariant::fromValue(xLimitedDouble(pProp->GetFontFactor(),1,100,10));
    _paramMp["label visible"]._value=(bool)pProp->GetLabelVisibility();
    _paramMp["label factor"]._value=QVariant::fromValue(xLimitedDouble(pProp->GetLabelFactor(),1,100,10));
    _paramMp["label format"]._value=QString(pProp->GetLabelFormat());
    _paramMp["label font"]._value.value<xVTextPropPtr>()->generateParamFromVtkTextProp(pProp->GetLabelTextProperty());
    _paramMp["label number"]._value=QVariant::fromValue(xLimitedInt(pProp->GetNumberOfLabels(),1,100));
    _paramMp["nice numbers"]._value=(bool)pProp->GetAdjustLabels();
    _paramMp["title"]._value=QString(pProp->GetTitle());
    _paramMp["title visible"]._value=(bool)pProp->GetTitleVisibility();
    _paramMp["title font"]._value.value<xVTextPropPtr>()->generateParamFromVtkTextProp(pProp->GetTitleTextProperty());
    _paramMp["title position"]._value=QVariant::fromValue(xLimitedDouble(pProp->GetTitlePosition(),1,100,10));
    _paramMp["ticks visible"]._value=(bool)pProp->GetTickVisibility();
    _paramMp["minor ticks"]._value=QVariant::fromValue(xLimitedInt(pProp->GetNumberOfMinorTicks(),1,100));
}

vtkAxisActor2D* xVAxisProp::generateVtkAxisPropFromParam(){
    // generate or update vtkPointer
    if (!pVtkAxisProp) pVtkAxisProp=vtkAxisActor2D::New();

    pVtkAxisProp->SetVisibility(_paramMp["visible"]._value.toBool());
    QPointF p=_paramMp["range"]._value.value<QPointF>();
    pVtkAxisProp->SetRange(p.x(),p.y());
    pVtkAxisProp->SetRulerMode(_paramMp["ruler mode"]._value.toBool());
    pVtkAxisProp->SetDragable(_paramMp["dragable"]._value.toBool());
    pVtkAxisProp->SetPickable(_paramMp["pickable"]._value.toBool());
    pVtkAxisProp->SetHeight(_paramMp["height"]._value.value<xLimitedDouble>()._value);
    pVtkAxisProp->SetWidth(_paramMp["width"]._value.value<xLimitedDouble>()._value);
    pVtkAxisProp->SetLayerNumber(_paramMp["layer"]._value.value<xLimitedInt>()._value);
    pVtkAxisProp->SetFontFactor(_paramMp["font factor"]._value.value<xLimitedDouble>()._value);
    pVtkAxisProp->SetLabelVisibility(_paramMp["label visible"]._value.toBool());
    pVtkAxisProp->SetLabelFactor(_paramMp["label factor"]._value.value<xLimitedDouble>()._value);
    pVtkAxisProp->SetLabelFormat(_paramMp["label format"]._value.toString().toLatin1());
    pVtkAxisProp->SetLabelTextProperty(_paramMp["label font"]._value.value<xVTextPropPtr>()->pVtkTextProp);
    pVtkAxisProp->SetNumberOfLabels(_paramMp["label number"]._value.value<xLimitedInt>()._value);
    pVtkAxisProp->SetAdjustLabels(_paramMp["nice numbers"]._value.toBool());
    pVtkAxisProp->SetTitle(_paramMp["title"]._value.toString().toLatin1());
    pVtkAxisProp->SetTitleVisibility(_paramMp["title visible"]._value.toBool());
    pVtkAxisProp->SetTitleTextProperty(_paramMp["title font"]._value.value<xVTextPropPtr>()->pVtkTextProp);
    pVtkAxisProp->SetTitlePosition(_paramMp["title position"]._value.value<xLimitedDouble>()._value);
    pVtkAxisProp->SetTickVisibility(_paramMp["ticks visible"]._value.toBool());
    pVtkAxisProp->SetNumberOfMinorTicks(_paramMp["minor ticks"]._value.value<xLimitedInt>()._value);
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
    setAutoFillBackground(false);
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
