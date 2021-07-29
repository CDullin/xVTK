#ifndef XV2DVISOBJ_H
#define XV2DVISOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisObj.h"

#include "vtkImageViewer2.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkImageMapToColors.h>
#include <vtkLegendScaleActor.h>

class xV2DVisObj:public xVGenVisObj
{
    Q_OBJECT
public:
    xV2DVisObj(const QString& txt="");
    xV2DVisObj(QDataStream &d);
    virtual void run() override;
    virtual void paramModified(const QString&) override;
    virtual void save (QDataStream&, bool _explicit=false) override;
    virtual void reset() override;

protected:

    void run_volume_data();
    void run_image_data();
    void run_cut_plane();
    void update_volume_data();
    void update_image_data();
    void update_cut_plane();

    vtkImageViewer2 *pImgViewer = nullptr;
    vtkMatrix4x4* pSliceOrientation = nullptr;
    vtkImageReslice *pSlicer = nullptr;
    vtkImageMapToColors* pColorMapper = nullptr;
    vtkImageActor *pIActor = nullptr;
    vtkActor2D *p2DActor = nullptr;

    QVTKWidget* pRenderWdgt=nullptr;
    vtkRenderer* pRenderer=nullptr;
    vtkRenderWindow *pRenderWin=nullptr;
    vtkRenderWindowInteractor *pRenderWindowInteractor=nullptr;
    vtkLegendScaleActor *pScaleBar = nullptr;
};


#endif // XV2DVISOBJ_H
