#ifndef XVTKDLG_H
#define XVTKDLG_H

#include <QDialog>
#include "xVObjects.h"
#include "xVPolyObj.h"
#include "xVTypes.h"
#include "xVConnectorObj.h"
#include "xvvismainwin.h"
#include <QGraphicsScene>
#include "vtkGUISupportQtModule.h"
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
//VTK_MODULE_INIT(vtkFixedPointVolumeRayCastMapper);

QT_BEGIN_NAMESPACE
namespace Ui { class xVTKDlg; }
QT_END_NAMESPACE

class xVGraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    xVGraphicsScene():QGraphicsScene(){}
signals:
    void dblClicked();
protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    {
        emit dblClicked();
        QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
    }
};

class xVTKDlg : public QDialog
{
    Q_OBJECT

public:
    xVTKDlg(QWidget *parent = nullptr);
    ~xVTKDlg();
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    xAbstractBasisObj* getObjById(const QString&);
    xConnector* getConnectorById(const QString&);

public slots:
    virtual void accept() override;
    virtual void reject() override;

protected slots:
    void connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE);
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);
    void removeObject(xAbstractBasisObj*);
    void clear(bool verbose=true);
    void save();
    void load();
    void verify();
    void run();
    void reset(bool verbose=true);
    void snapToGrid(xVObj_Basics* pVObj=nullptr);
    xVObj_Basics* baseObjFromId(const QString&);
    void deselectAll();
    void activateNext();
    void dispMemoryConsumption();

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

protected:
    void placeObjInScene(xVObj_Basics* pObj);
    void processRec(xVObj_Basics* pVObj);
    void generateHooks();
    void addVisWidget(QWidget*);
    void removeVisWidget(QWidget*);
    QString uniqueName(QString);

private:
    Ui::xVTKDlg *ui;

    bool _inConnectorSet = false;
    bool _paramInConnectorSet = false;
    bool _outConnectorSet = false;
    xVObj_Basics* pInEItem = nullptr;
    xVObj_Basics* pOutEItem = nullptr;

    QList <xAbstractBasisObj*> _objLst;
    xVVisMainWin *pVisMainWin=nullptr;
};
#endif // XVTKDLG_H
