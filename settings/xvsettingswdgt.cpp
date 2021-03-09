#include "xvsettingswdgt.h"
#include "ui_xvsettingswdgt.h"
#include "xvEvalCondition.h"

xVSettingsWdgt::xVSettingsWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVSettingsWdgt)
{
    ui->setupUi(this);

    generateDefault();
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
    qRegisterMetaType<QFileInfoPtr>("QFileInfoPtr");
    qRegisterMetaType<x3D_SAMPLE_POS>("x3D_SAMPLE_POS");
    qRegisterMetaType<xLimitedInt>("xLimitedInt");
    qRegisterMetaType<xLimitedDouble>("xLimitedDouble");
    qRegisterMetaType<xParamMap>("xParamMap");
    qRegisterMetaType<xVEvalCondition>("xVEvalCondition");

    qRegisterMetaTypeStreamOperators<QFileInfo>("QFileInfo");
    qRegisterMetaTypeStreamOperators<QPointF>("QPointF");
    qRegisterMetaTypeStreamOperators<x3D_SAMPLE_POS>("x3D_SAMPLE_POS");
    qRegisterMetaTypeStreamOperators<vtkPiecewiseFunctionPtr>("vtkPiecewiseFunctionPtr");
    qRegisterMetaTypeStreamOperators<vtkColorTransferFunctionPtr>("vtkColorTransferFunctionPtr");
    qRegisterMetaTypeStreamOperators<xLimitedInt>("xLimitedInt");
    qRegisterMetaTypeStreamOperators<xLimitedDouble>("xLimitedDouble");
    qRegisterMetaTypeStreamOperators<xParamMap>("xParamMap");
    qRegisterMetaTypeStreamOperators<xVEvalCondition>("xVEvalCondition");

    ::_settings["snap to grid"]._id=0;
    ::_settings["snap to grid"]._value=true;
    ::_settings["grid resolution"]._id=1;
    ::_settings["grid resolution"]._value=25.0;
    ::_settings["last mesh file"]._id=2;
    ::_settings["last mesh file"]._value="/home/heimdall/development/sample_data/sphere.ply";
    ::_settings["last volume file"]._id=3;
    ::_settings["last volume file"]._value="/home/heimdall/development/sample_data/embryo.raw";
    ::_settings["export dashboard path"]._id=4;
    ::_settings["export dashboard path"]._value="/home/heimdall/development/sample_data/dash_boards";
    ::_settings["progress scaling factor"]._id=5;
    ::_settings["progress scaling factor"]._value=100.0f;

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
}

xVSettingsWdgt::~xVSettingsWdgt()
{
    delete ui;
}
