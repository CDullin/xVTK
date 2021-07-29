QT       += core gui concurrent multimedia serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    3dview/xV3DRenderWindowInteractor.cpp \
    3dview/xV3DVisObj.cpp \
    3dview/xVDefine3DTransformationToolWdgt.cpp \
    3dview/xVVtkWidget.cpp \
    arduino/xVArduinoComObj.cpp \
    arduino/xVArduinoConnectObj.cpp \
    arduino/xVArduinoThread.cpp \
    camera/xVCamObj.cpp \
    camera/xVCamPropObj.cpp \
    dashboard/xVDashboardToolBar.cpp \
    dialogs/xvloaddlg.cpp \
    dialogs/xvsavedlg.cpp \
    fuzzy/xVDefuzzificationObj.cpp \
    fuzzy/xVFuzzificationObj.cpp \
    fuzzy/xVFuzzyRuleBaseObj.cpp \
    graph/xVGraphWdgt.cpp \
    help/xVHelpBrowser.cpp \
    histogram/xVHistoHValueAxis.cpp \
    histogram/xVHistoNodeItem.cpp \
    histogram/xVHistoPixmapItem.cpp \
    histogram/xVHistoVLine.cpp \
    histogram/xVHistoVValueAxis.cpp \
    histogram/xvhistodlg.cpp \
    histogram/xvhistodlg_color.cpp \
    histogram/xvhistodlg_opacity.cpp \
    histogram/xvhistodlg_pixmaps.cpp \
    histogram/xvhistodlg_tools.cpp \
    main.cpp \
    objects/xV2DVisObj.cpp \
    objects/xVAbstractBaseObj.cpp \
    objects/xVBoxObj.cpp \
    objects/xVCallDashboardObj.cpp \
    objects/xVCallExternalObj.cpp \
    objects/xVConnector.cpp \
    objects/xVConnectorObj.cpp \
    objects/xVCustomGraphicItems.cpp \
    objects/xVDashboardView.cpp \
    objects/xVEndObj.cpp \
    objects/xVExportObj.cpp \
    objects/xVGenFilterObj.cpp \
    objects/xVGenImpObj.cpp \
    objects/xVGenUserDlgObj.cpp \
    objects/xVGenVisObj.cpp \
    objects/xVGenVisPropObj.cpp \
    objects/xVGraphObj.cpp \
    objects/xVHisto.cpp \
    objects/xVIFObj.cpp \
    objects/xVImageObj.cpp \
    objects/xVImageStackObj.cpp \
    objects/xVImportCVSObj.cpp \
    objects/xVLSIFilterObj.cpp \
    objects/xVMathObj.cpp \
    objects/xVMeasurementRegion.cpp \
    objects/xVMeshVisPropObj.cpp \
    objects/xVMorphFilterObj.cpp \
    objects/xVObjects.cpp \
    objects/xVPlaneObj.cpp \
    objects/xVPolyObj.cpp \
    objects/xVPolygonizationFilterObj.cpp \
    objects/xVStartObj.cpp \
    objects/xVTrafficLightObj.cpp \
    objects/xVUserTableImportDlgObj.cpp \
    objects/xVVarDefinitionObj.cpp \
    objects/xVVolObj.cpp \
    objects/xVVolumeVisPropObj.cpp \
    objects/xVWaitObj.cpp \
    objects/xvcreateobjtreewdgt.cpp \
    objects/xvusertableruntimedlg.cpp \
    report/xVReport.cpp \
    report/xVReportObj.cpp \
    report/xVReportWdgt.cpp \
    report/xVReportWdgtToolBar.cpp \
    session/xVSessionDlg.cpp \
    settings/xVAxisDefinitionDlg.cpp \
    settings/xVAxisPropertyDlgItem.cpp \
    settings/xVBoolDlgItem.cpp \
    settings/xVCVSImportPreviewDlg.cpp \
    settings/xVColorDlgItem.cpp \
    settings/xVDoubleValueDlgItem.cpp \
    settings/xVEquationDlgItem.cpp \
    settings/xVFileImportDlgItem.cpp \
    settings/xVFontDefinitionDlg.cpp \
    settings/xVIntValueDlgItem.cpp \
    settings/xVLUTFuncDlgItem.cpp \
    settings/xVOpacityFuncDlgItem.cpp \
    settings/xVPointDlgItem.cpp \
    settings/xVSamplePosDlgItem.cpp \
    settings/xVTextPropertyDlgItem.cpp \
    settings/xVUserTableDefinitionDlgItem.cpp \
    settings/xVVector3DDlgItem.cpp \
    settings/xrcolorpickdlg.cpp \
    settings/xvequationdesigndlg.cpp \
    settings/xvproptable.cpp \
    settings/xvpropwdgt.cpp \
    settings/xvsettingswdgt.cpp \
    settings/xvusertabledefinitiondlg.cpp \
    sidepanel/xvsidepanelwdgt.cpp \
    tools/xVClock.cpp \
    tools/xVGenVolIO.cpp \
    tools/xVIOVFF.cpp \
    tools/xVIOVOL.cpp \
    tools/xVIOVOX.cpp \
    tools/xVTable.cpp \
    tools/xVTypes.cpp \
    tools/xvEvalCondition.cpp \
    tools/xviconfactory.cpp \
    visualization/xvvismainwin.cpp \
    xVTKDlg_compile.cpp \
    xvtdlg_io.cpp \
    xvtkdlg.cpp \
    xvtkdlg_connector.cpp \
    xvtkdlg_reset.cpp \
    xvtkdlg_run.cpp

HEADERS += \
    3dview/xV3DRenderWindowInteractor.h \
    3dview/xV3DVisObj.h \
    3dview/xVDefine3DTransformationToolWdgt.h \
    3dview/xVVtkWidget.h \
    arduino/xVArduinoComObj.h \
    arduino/xVArduinoConnectObj.h \
    arduino/xVArduinoThread.h \
    camera/xVCamObj.h \
    camera/xVCamPropObj.h \
    dashboard/xVDashboardToolBar.h \
    dialogs/xvloaddlg.h \
    dialogs/xvsavedlg.h \
    fuzzy/xVDefuzzificationObj.h \
    fuzzy/xVFuzzificationObj.h \
    fuzzy/xVFuzzyRuleBaseObj.h \
    graph/xVGraphWdgt.h \
    help/xVHelpBrowser.h \
    histogram/xVHistoDlg_types.h \
    histogram/xVHistoHValueAxis.h \
    histogram/xVHistoNodeItem.h \
    histogram/xVHistoPixmapItem.h \
    histogram/xVHistoVLine.h \
    histogram/xVHistoVValueAxis.h \
    histogram/xvhistodlg.h \
    objects/xV2DVisObj.h \
    objects/xVAbstractBaseObj.h \
    objects/xVArduinoComObj.h \
    objects/xVArduinoConnectObj.h \
    objects/xVBoxObj.h \
    objects/xVCallDashboardObj.h \
    objects/xVCallExternalObj.h \
    objects/xVCamObj.h \
    objects/xVCamPropObj.h \
    objects/xVConnector.h \
    objects/xVConnectorObj.h \
    objects/xVCustomGraphicItems.h \
    objects/xVDashboardView.h \
    objects/xVEndObj.h \
    objects/xVExportObj.h \
    objects/xVFuzzificationObj.h \
    objects/xVGenFilterObj.h \
    objects/xVGenImpObj.h \
    objects/xVGenUserDlgObj.h \
    objects/xVGenVisObj.h \
    objects/xVGenVisPropObj.h \
    objects/xVGraphObj.h \
    objects/xVGraphicsScene.h \
    objects/xVHisto.h \
    objects/xVIFObj.h \
    objects/xVImageObj.h \
    objects/xVImageStackObj.h \
    objects/xVImportCVSObj.h \
    objects/xVLSIFilterObj.h \
    objects/xVMathObj.h \
    objects/xVMeasurementRegion.h \
    objects/xVMeshVisPropObj.h \
    objects/xVMorphFilterObj.h \
    objects/xVObjectTypes.h \
    objects/xVObjects.h \
    objects/xVPlaneObj.h \
    objects/xVPolyObj.h \
    objects/xVPolygonizationFilterObj.h \
    objects/xVReportObj.h \
    objects/xVStartObj.h \
    objects/xVTrafficLightObj.h \
    objects/xVUserTableImportDlgObj.h \
    objects/xVVarDefinitionObj.h \
    objects/xVVolObj.h \
    objects/xVVolumeVisPropObj.h \
    objects/xVWaitObj.h \
    objects/xvcreateobjtreewdgt.h \
    objects/xvusertableruntimedlg.h \
    report/xVReport.h \
    report/xVReportObj.h \
    report/xVReportWdgt.h \
    report/xVReportWdgtToolBar.h \
    session/xVSessionDlg.h \
    settings/xVAxisDefinitionDlg.h \
    settings/xVAxisPropertyDlgItem.h \
    settings/xVBoolDlgItem.h \
    settings/xVCVSImportPreviewDlg.h \
    settings/xVColorDlgItem.h \
    settings/xVCustomTableItems.h \
    settings/xVDoubleValueDlgItem.h \
    settings/xVEquationDlgItem.h \
    settings/xVFileImportDlgItem.h \
    settings/xVFontDefinitionDlg.h \
    settings/xVIntValueDlgItem.h \
    settings/xVLUTFuncDlgItem.h \
    settings/xVOpacityFuncDlgItem.h \
    settings/xVPointDlgItem.h \
    settings/xVSamplePosDlgItem.h \
    settings/xVTextPropertyDlgItem.h \
    settings/xVUserTableDefinitionDlgItem.h \
    settings/xVVector3DDlgItem.h \
    settings/xrcolorpickdlg.h \
    settings/xvequationdesigndlg.h \
    settings/xvproptable.h \
    settings/xvpropwdgt.h \
    settings/xvsettingswdgt.h \
    settings/xvusertabledefinitiondlg.h \
    sidepanel/xvsidepanelwdgt.h \
    tools/xVClock.h \
    tools/xVGenVolIO.h \
    tools/xVIOVFF.h \
    tools/xVIOVOL.h \
    tools/xVIOVOX.h \
    tools/xVTable.h \
    tools/xVTypes.h \
    tools/xvEvalCondition.h \
    tools/xviconfactory.h \
    visualization/xvvismainwin.h \
    xVGraphWdgt.h \
    xVHelpBrowser.h \
    xvtkdlg.h \
    xvvismainwin.h

FORMS += \
    3dview/xVDefine3DTransformationToolWdgt.ui \
    dashboard/xVDashboardToolBar.ui \
    dialogs/xvloaddlg.ui \
    dialogs/xvsavedlg.ui \
    objects/xvusertableruntimedlg.ui \
    report/xVReportWdgtToolBar.ui \
    session/xVSessionDlg.ui \
    settings/xVAxisDefinitionDlg.ui \
    settings/xVCVSImportPreviewDlg.ui \
    settings/xVFontDefinitionDlg.ui \
    settings/xrcolorpickdlg.ui \
    settings/xvequationdesigndlg.ui \
    settings/xvpropwdgt.ui \
    settings/xvsettingswdgt.ui \
    settings/xvusertabledefinitiondlg.ui \
    visualization/xvvismainwin.ui \
    xvtkdlg.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    xVTK_resources.qrc

LIBS += -L$$PWD/../../../../../opt/pylon/lib/ -lpylonbase
LIBS += -L$$PWD/../../../../../opt/pylon/lib/ -lGenApi_gcc_v3_1_Basler_pylon
LIBS += -L$$PWD/../../../../../opt/pylon/lib/ -lGCBase_gcc_v3_1_Basler_pylon
LIBS += -L$$PWD/../../../../../opt/pylon/lib/ -lpylonc
LIBS += -L$$PWD/../../../../../opt/pylon/lib/ -lpylonutility
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonCore-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonTransforms-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOGeometry-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOLegacy-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonExecutionModel-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonDataModel-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOXML-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOPLY-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingCore-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkFiltersSources-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingCore-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingContextOpenGL2-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingOpenGL2-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingVolume-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingVolumeOpenGL2-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingQt-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingFreeType-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkInteractionStyle-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkGUISupportQt-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkGUISupportQtOpenGL-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOImage-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonMath-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkImagingCore-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkFiltersGeneral-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingAnnotation-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkInteractionWidgets-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkImagingGeneral-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonMisc-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkFiltersCore-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOCore-8.0
LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkFiltersModeling-8.0
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmimage
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmimgle
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -lcmr
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmdata
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmdsig
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmect
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmfg
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmiod
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmjpeg
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmjpls
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmnet
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmpmap
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmpstat
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmqrdb
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmrt
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmseg
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmsr
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmtkcharls
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmtls
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmtract
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -ldcmwlm
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -li2d
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -lijg12
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -lijg16
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -lijg8
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -loflog
LIBS += -L$$PWD/../../3rd_party/dcmtk-build/lib/ -lofstd
LIBS += -lz

INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/DataModel
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/ExecutionModel
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Transforms
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/System
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Math
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Color
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Misc
INCLUDEPATH += $$PWD/../VTK-8.0.1_source
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/Sources
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/General
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/Imaging
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/Modeling
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/OpenGL2
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/Volume
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/VolumeOpenGL2
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/LOD
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/Annotation
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Utilities/KWIML
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Utilities/KWSys
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO/XML
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO/PLY
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO/Legacy
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO/Geometry
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/IO/Image
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/GUISupport
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/GUISupport/Qt
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/GUISupport/QtOpenGL
INCLUDEPATH += $$PWD/../VTK8-build/GUISupport/QtOpenGL
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Interaction
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Interaction/Image
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Interaction/Widgets
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Interaction/Style
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Imaging/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Imaging/General

INCLUDEPATH += $$PWD/dialogs
INCLUDEPATH += $$PWD/objects
INCLUDEPATH += $$PWD/settings
INCLUDEPATH += $$PWD/tools
INCLUDEPATH += $$PWD/histogram
INCLUDEPATH += $$PWD/report
INCLUDEPATH += $$PWD/arduino
INCLUDEPATH += $$PWD/dashboard
INCLUDEPATH += $$PWD/camera
INCLUDEPATH += $$PWD/visualization
INCLUDEPATH += $$PWD/sidepanel
INCLUDEPATH += $$PWD/session
INCLUDEPATH += $$PWD/help
INCLUDEPATH += $$PWD/fuzzy
INCLUDEPATH += $$PWD/graph
INCLUDEPATH += $$PWD/3dview

INCLUDEPATH += $$PWD/../../../../../opt/pylon/include
INCLUDEPATH += $$PWD/../../../../../opt/pylon/include/GenApi

DEPENDPATH += $$PWD/../../../../../opt/pylon/include/GenApi
DEPENDPATH += $$PWD/../../../../../opt/pylon/include
DEPENDPATH += $$PWD/dialogs
DEPENDPATH += $$PWD/objects
DEPENDPATH += $$PWD/settings
DEPENDPATH += $$PWD/tools
DEPENDPATH += $$PWD/histogram
DEPENDPATH += $$PWD/arduino
DEPENDPATH += $$PWD/report
DEPENDPATH += $$PWD/dashboard
DEPENDPATH += $$PWD/camera
DEPENDPATH += $$PWD/visualization
DEPENDPATH += $$PWD/sidepanel
DEPENDPATH += $$PWD/session
DEPENDPATH += $$PWD/help
DEPENDPATH += $$PWD/fuzzy
DEPENDPATH += $$PWD/graph
DEPENDPATH += $$PWD/../VTK-8.0.1_source/Common/DataModel

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtksys-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtksys-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtksys-8.0

INCLUDEPATH += $$PWD/../VTK8-build
DEPENDPATH += $$PWD/../VTK8-build

DISTFILES += \
    documents/VTKUsersGuide.pdf \
    documents/notes \
    images/colormap.png \
    images/seamless-circuit-board.png \
    images/vtk-logo.png

