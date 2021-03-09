QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogs/xvloaddlg.cpp \
    dialogs/xvsavedlg.cpp \
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
    objects/xV3DVisObj.cpp \
    objects/xVCallExternalObj.cpp \
    objects/xVConnector.cpp \
    objects/xVConnectorObj.cpp \
    objects/xVCustomGraphicItems.cpp \
    objects/xVGenFilterObj.cpp \
    objects/xVGenImpObj.cpp \
    objects/xVGenUserDlgObj.cpp \
    objects/xVGenVisObj.cpp \
    objects/xVGenVisPropObj.cpp \
    objects/xVHisto.cpp \
    objects/xVIFObj.cpp \
    objects/xVMathObj.cpp \
    objects/xVMeshVisPropObj.cpp \
    objects/xVObjects.cpp \
    objects/xVPolyObj.cpp \
    objects/xVStartObj.cpp \
    objects/xVTrafficLightObj.cpp \
    objects/xVUserTableImportDlgObj.cpp \
    objects/xVVarDefinitionObj.cpp \
    objects/xVVolObj.cpp \
    objects/xVVolumeVisPropObj.cpp \
    objects/xvcreateobjtreewdgt.cpp \
    objects/xvusertableruntimedlg.cpp \
    settings/xVBoolDlgItem.cpp \
    settings/xVColorDlgItem.cpp \
    settings/xVDoubleValueDlgItem.cpp \
    settings/xVFileImportDlgItem.cpp \
    settings/xVIntValueDlgItem.cpp \
    settings/xVLUTFuncDlgItem.cpp \
    settings/xVOpacityFuncDlgItem.cpp \
    settings/xVSamplePosDlgItem.cpp \
    settings/xVUserTableDefinitionDlgItem.cpp \
    settings/xVVector3DDlgItem.cpp \
    settings/xrcolorpickdlg.cpp \
    settings/xvproptable.cpp \
    settings/xvpropwdgt.cpp \
    settings/xvsettingswdgt.cpp \
    settings/xvusertabledefinitiondlg.cpp \
    tools/xVTypes.cpp \
    tools/xvEvalCondition.cpp \
    tools/xviconfactory.cpp \
    xvsidepanelwdgt.cpp \
    xvtdlg_io.cpp \
    xvtkdlg.cpp \
    xvtkdlg_connector.cpp \
    xvtkdlg_reset.cpp \
    xvtkdlg_run.cpp \
    xvvismainwin.cpp

HEADERS += \
    dialogs/xvloaddlg.h \
    dialogs/xvsavedlg.h \
    histogram/xVHistoDlg_types.h \
    histogram/xVHistoHValueAxis.h \
    histogram/xVHistoNodeItem.h \
    histogram/xVHistoPixmapItem.h \
    histogram/xVHistoVLine.h \
    histogram/xVHistoVValueAxis.h \
    histogram/xvhistodlg.h \
    objects/xV2DVisObj.h \
    objects/xV3DVisObj.h \
    objects/xVCallExternalObj.h \
    objects/xVConnector.h \
    objects/xVConnectorObj.h \
    objects/xVCustomGraphicItems.h \
    objects/xVGenFilterObj.h \
    objects/xVGenImpObj.h \
    objects/xVGenUserDlgObj.h \
    objects/xVGenVisObj.h \
    objects/xVGenVisPropObj.h \
    objects/xVHisto.h \
    objects/xVIFObj.h \
    objects/xVMathObj.h \
    objects/xVMeshVisPropObj.h \
    objects/xVObjectTypes.h \
    objects/xVObjects.h \
    objects/xVPolyObj.h \
    objects/xVStartObj.h \
    objects/xVTrafficLightObj.h \
    objects/xVUserTableImportDlgObj.h \
    objects/xVVarDefinitionObj.h \
    objects/xVVolObj.h \
    objects/xVVolumeVisPropObj.h \
    objects/xvcreateobjtreewdgt.h \
    objects/xvusertableruntimedlg.h \
    settings/xVBoolDlgItem.h \
    settings/xVColorDlgItem.h \
    settings/xVCustomTableItems.h \
    settings/xVDoubleValueDlgItem.h \
    settings/xVFileImportDlgItem.h \
    settings/xVIntValueDlgItem.h \
    settings/xVLUTFuncDlgItem.h \
    settings/xVOpacityFuncDlgItem.h \
    settings/xVSamplePosDlgItem.h \
    settings/xVUserTableDefinitionDlgItem.h \
    settings/xVVector3DDlgItem.h \
    settings/xrcolorpickdlg.h \
    settings/xvproptable.h \
    settings/xvpropwdgt.h \
    settings/xvsettingswdgt.h \
    settings/xvusertabledefinitiondlg.h \
    tools/xVTypes.h \
    tools/xvEvalCondition.h \
    tools/xviconfactory.h \
    xvsidepanelwdgt.h \
    xvtkdlg.h \
    xvvismainwin.h

FORMS += \
    dialogs/xvloaddlg.ui \
    dialogs/xvsavedlg.ui \
    objects/xvusertableruntimedlg.ui \
    settings/xrcolorpickdlg.ui \
    settings/xvpropwdgt.ui \
    settings/xvsettingswdgt.ui \
    settings/xvusertabledefinitiondlg.ui \
    xvtkdlg.ui \
    xvvismainwin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    xVTK_resources.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkCommonCore-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkCommonCore-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonCore-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkIOGeometry-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkIOGeometry-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOGeometry-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkIOLegacy-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkIOLegacy-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOLegacy-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkCommonExecutionModel-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkCommonExecutionModel-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonExecutionModel-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkCommonDataModel-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkCommonDataModel-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonDataModel-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkIOXML-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkIOXML-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOXML-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkIOPLY-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkIOPLY-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOPLY-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingCore-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingCore-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingCore-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkFiltersSources-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkFiltersSources-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkFiltersSources-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingCore-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingCore-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingCore-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingContextOpenGL2-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingContextOpenGL2-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingContextOpenGL2-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingOpenGL2-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingOpenGL2-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingOpenGL2-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingVolume-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingVolume-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingVolume-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingVolumeOpenGL2-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingVolumeOpenGL2-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingVolumeOpenGL2-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingQt-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingQt-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingQt-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkRenderingFreeType-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkRenderingFreeType-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkRenderingFreeType-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkInteractionStyle-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkInteractionStyle-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkInteractionStyle-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkGUISupportQt-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkGUISupportQt-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkGUISupportQt-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkGUISupportQtOpenGL-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkGUISupportQtOpenGL-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkGUISupportQtOpenGL-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkIOImage-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkIOImage-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkIOImage-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkCommonMath-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkCommonMath-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkCommonMath-8.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtkImagingCore-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtkImagingCore-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtkImagingCore-8.0

INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/DataModel
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/ExecutionModel
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/System
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Math
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Common/Color
INCLUDEPATH += $$PWD/../VTK-8.0.1_source
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/Sources
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Filters/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/OpenGL2
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/Core
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/Volume
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/VolumeOpenGL2
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Rendering/LOD
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
INCLUDEPATH += $$PWD/../VTK-8.0.1_source/Imaging/Core


INCLUDEPATH += $$PWD/dialogs
INCLUDEPATH += $$PWD/objects
INCLUDEPATH += $$PWD/settings
INCLUDEPATH += $$PWD/tools
INCLUDEPATH += $$PWD/histogram

DEPENDPATH += $$PWD/dialogs
DEPENDPATH += $$PWD/objects
DEPENDPATH += $$PWD/settings
DEPENDPATH += $$PWD/tools
DEPENDPATH += $$PWD/histogram

DEPENDPATH += $$PWD/../VTK-8.0.1_source/Common/DataModel


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/release/ -lvtksys-8.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../VTK8-build/lib/debug/ -lvtksys-8.0
else:unix: LIBS += -L$$PWD/../VTK8-build/lib/ -lvtksys-8.0

INCLUDEPATH += $$PWD/../VTK8-build
DEPENDPATH += $$PWD/../VTK8-build

DISTFILES += \
    documents/VTKUsersGuide.pdf \
    images/colormap.png \
    images/seamless-circuit-board.png \
    images/vtk-logo.png
