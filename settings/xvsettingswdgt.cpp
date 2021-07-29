#include "xvsettingswdgt.h"
#include "ui_xvsettingswdgt.h"
#include "xvEvalCondition.h"
#include "xVTextPropertyDlgItem.h"
#include "xVAxisPropertyDlgItem.h"
#include "xVTable.h"
#include "xVTypes.h"

xVSettingsWdgt::xVSettingsWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVSettingsWdgt)
{
    ui->setupUi(this);

    generateDefault();
    ui->pSettingsTable->updateTable(&::_settings,nullptr);
}

void xVSettingsWdgt::generateDefault()
{
    qRegisterMetaType<vtkActorPtr>("vtkActorPtr");
    qRegisterMetaType<vtkVolumePtr>("vtkVolumePtr");
    qRegisterMetaType<vtkPolyDataPtr>("vtkPolyDataPtr");
    qRegisterMetaType<vtkPolyDataMapperPtr>("vtkPolyDataMapperPtr");
    qRegisterMetaType<vtkImageReaderPtr>("vtkImageReaderPtr");
    qRegisterMetaType<vtkImageDataPtr>("vtkImageDataPtr");
    qRegisterMetaType<vtkPiecewiseFunctionPtr>("vtkPiecewiseFunctionPtr");
    qRegisterMetaType<vtkColorTransferFunctionPtr>("vtkColorTransferFunctionPtr");
    qRegisterMetaType<xFileName>("xFileName");
    qRegisterMetaType<x3D_SAMPLE_POS>("x3D_SAMPLE_POS");
    qRegisterMetaType<xLimitedInt>("xLimitedInt");
    qRegisterMetaType<xLimitedDouble>("xLimitedDouble");
    qRegisterMetaType<xParamMap>("xParamMap");
    qRegisterMetaType<xVEvalCondition>("xVEvalCondition");
    qRegisterMetaType<xVTablePtr>("xVTablePtr");
    qRegisterMetaType<xVTextPropPtr>("xVTextPropPtr");
    qRegisterMetaType<xVAxisPropPtr>("xVAxisPropPtr");

    qRegisterMetaTypeStreamOperators<xVTablePtr>("xVTablePtr");
    qRegisterMetaTypeStreamOperators<xFileName>("xFileName");
    qRegisterMetaTypeStreamOperators<QPointF>("QPointF");
    qRegisterMetaTypeStreamOperators<x3D_SAMPLE_POS>("x3D_SAMPLE_POS");
    qRegisterMetaTypeStreamOperators<vtkPiecewiseFunctionPtr>("vtkPiecewiseFunctionPtr");
    qRegisterMetaTypeStreamOperators<vtkColorTransferFunctionPtr>("vtkColorTransferFunctionPtr");
    qRegisterMetaTypeStreamOperators<xLimitedInt>("xLimitedInt");
    qRegisterMetaTypeStreamOperators<xLimitedDouble>("xLimitedDouble");
    qRegisterMetaTypeStreamOperators<xParamMap>("xParamMap");
    qRegisterMetaTypeStreamOperators<xVEvalCondition>("xVEvalCondition");
    qRegisterMetaTypeStreamOperators<xVTextPropPtr>("xVTextPropPtr");
    qRegisterMetaTypeStreamOperators<xVAxisPropPtr>("xVAxisPropPtr");
    qRegisterMetaTypeStreamOperators<vtkImageDataPtr>("vtkImageDataPtr");
    qRegisterMetaTypeStreamOperators<vtkVolumePtr>("vtkVolumePtr");

    ::_settings["snap to grid"]._id=0;
    ::_settings["snap to grid"]._value=true;
    ::_settings["grid resolution"]._id=1;
    ::_settings["grid resolution"]._value=QVariant::fromValue(xLimitedInt(25,1,100));
    ::_settings["last mesh file"]._id=2;
    ::_settings["last mesh file"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"/home/heimdall/development/sample_data/sphere.ply"));
    ::_settings["last volume file"]._id=3;
    ::_settings["last volume file"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"/home/heimdall/development/sample_data/embryo.raw"));
    ::_settings["export dashboard path"]._id=4;
    ::_settings["export dashboard path"]._value=QVariant::fromValue(xFileName(xFileName::FN_OUTPUT_DIR,"/home/heimdall/development/sample_data/dash_boards"));
    ::_settings["progress scaling factor"]._id=5;
    ::_settings["progress scaling factor"]._value=QVariant::fromValue(xLimitedDouble(100.0,1.0,100.0,10));
    ::_settings["last dashboard file"]._id=6;
    ::_settings["last dashboard file"]._value=QVariant::fromValue(xFileName(xFileName::FN_OUTPUT_FILE,"/home/heimdall/development/sample_data/dash_boards"));
    ::_settings["auto save dashboard"]._id=7;
    ::_settings["auto save dashboard"]._value=true;
    ::_settings["auto save dashboard interval [s]"]._id=8;
    ::_settings["auto save dashboard interval [s]"]._value=QVariant::fromValue(xLimitedInt(60,1,600));
    ::_settings["temp folder"]._id=9;
    ::_settings["temp folder"]._value=QVariant::fromValue(xFileName(xFileName::FN_OUTPUT_DIR,"/home/heimdall/tmp/xVTK"));
    ::_settings["last report file"]._id=10;
    ::_settings["last report file"]._value="";
    ::_settings["report auto save"]._id=11;
    ::_settings["report auto save"]._value=true;
    ::_settings["report auto save interval [s]"]._id=12;
    ::_settings["report auto save interval [s]"]._value=QVariant::fromValue(xLimitedInt(60,1,600));
    ::_settings["play sounds"]._id=13;
    ::_settings["play sounds"]._value = false;
    ::_settings["master volume"]._id=14;
    ::_settings["master volume"]._value = QVariant::fromValue(xLimitedDouble(0.5,0.0,1.0,10));

    // set limits
    _limitMp["line thickness"]._lowerLimit=0.2;
    _limitMp["line thickness"]._upperLimit=25.0;
    _limitMp["line thickness"]._reduction=10.0f;
    _limitMp["point size"]._lowerLimit=0.2;
    _limitMp["point size"]._upperLimit=25.0;
    _limitMp["point size"]._reduction=10.0f;
    _limitMp["ambient light power"]._lowerLimit=0.0f;_limitMp["ambient light power"]._upperLimit=5.0f;_limitMp["ambient light power"]._reduction=10.0f;
    _limitMp["diffuse light power"]._lowerLimit=0.0f;_limitMp["diffuse light power"]._upperLimit=5.0f;_limitMp["diffuse light power"]._reduction=10.0f;
    _limitMp["specular light power"]._lowerLimit=0.0f;_limitMp["specular light power"]._upperLimit=5.0f;_limitMp["specular light power"]._reduction=10.0f;

    _optionLsts["mesh visualization style"] << "points" << "mesh" << "face";
    _optionLsts["volume file type"] << "vox" << "vff" << "vtk" << "vol" << "raw";
    _optionLsts["data type"] << "int8" << "uint8" << "int16" << "uint16" << "int32" << "uint32" << "float" << "double";
    _optionLsts["volume visualization style"] << "additiv" << "average" << "composite" << "maximum" << "minimum";
    _optionLsts["2dview orientation"] << "xy-plane" << "xz-plane" << "yz-plane" << "free-plane";
    _optionLsts["histogram mode"] << "relative" << "logarithmic" << "absolute";
    _optionLsts["grab strategy"] << "single image" << "defined number of images" << "continous acquistion";
    _optionLsts["export file type"] << "tif" << "png" << "bmp" << "jpg" << "mha" << "vox" << "dcm" << "stl" << "obj" << "ply";
}

xVSettingsWdgt::~xVSettingsWdgt()
{
    delete ui;
}
