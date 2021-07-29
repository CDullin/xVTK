#ifndef XV3DVISOBJ_H
#define XV3DVISOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenVisObj.h"
#include "xVVtkWidget.h"

#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkLegendScaleActor.h>
#include <vtkLightKit.h>

class xV3DVisObj:public xVGenVisObj
{
    Q_OBJECT
public:
    xV3DVisObj(const QString& txt="");
    xV3DVisObj(QDataStream &d);
    virtual void run() override;
    virtual void save(QDataStream&, bool _explicit=false) override;
    virtual void reset() override;

public slots:
    void updateRenderer(){}
    virtual void paramModified(const QString& txt="") override;

protected:
    xVVtkWidget* pRenderWdgt=nullptr;
    vtkRenderer* pRenderer=nullptr;
    vtkRenderWindow *pRenderWin=nullptr;
    vtkRenderWindowInteractor *pRenderWindowInteractor=nullptr;
    vtkOrientationMarkerWidget *pOrientationMaker=nullptr;
    vtkAxesActor* pAxes= nullptr;
    vtkRenderWindowInteractor* pAxesInteractor = nullptr;
    vtkLegendScaleActor *pScaleBar = nullptr;
};

typedef QVTKInteractor* QVTKInteractorPtr;
Q_DECLARE_METATYPE(QVTKInteractorPtr);

#endif // XV3DVISOBJ_H

