#ifndef XVTKDLG_H
#define XVTKDLG_H

#include <QGraphicsSceneMouseEvent>
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

//!
//! \brief The xVGraphicsScene class
//!

class xVGraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    xVGraphicsScene():QGraphicsScene(){}
signals:
    void dblClicked();
    void rDblClicked();
protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    {
        if (mouseEvent->button()==Qt::LeftButton)
            emit dblClicked();
        if (mouseEvent->button()==Qt::RightButton)
            emit rDblClicked();
        QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
    }
};

//!
//! \brief The xVTKDlg class
//!
class xVTKDlg : public QDialog
{
    Q_OBJECT

public:
    xVTKDlg(QWidget *parent = nullptr);
    ~xVTKDlg();
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    xVAbstractBaseObj* getObjById(const QString&);
    xConnector* getConnectorById(const QString&);

public slots:
    virtual void accept() override;
    virtual void reject() override;

protected slots:
    void connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE);
    //!
    //! \brief KSlot
    //! is a general purpose slot to allow signal handling between different parts of the software.
    //! Signals are caught by the main dialog and broadcasted again
    //! \param t
    //! \param pData
    //!
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);
    void removeObject(xVAbstractBaseObj*);
    //!
    //! \brief clear
    //! removes all elements from the dashboard and restores a start and an end object
    //! \param verbose
    //!
    void clear(bool verbose=true);
    //!
    //! \brief save
    //! allows exporting the dashboard into a target folder. Data sets used in the dashboard can either be copied as well (explicite) or
    //! only the relative path is stored
    //!
    void save();
    //!
    //! \brief load
    //! restores a saved dashboard
    //!
    void load();
    void verify();
    //!
    //! \brief step
    //! is used to start or advance the progress in the dashboard by one. Its mainly for debugging purposes.
    //! \param _force
    //!
    void step(bool _force=false);
    //!
    //! \brief run
    //! excutes the entire dashboard until no unprocessed objects are found.
    //!
    void run();
    //!
    //! \brief reset
    //! clears the memory of all objects and put them back into unprocessed state.
    //! \param verbose
    //!
    void reset(bool verbose=true);
    void snapToGrid(xVObj_Basics* pVObj=nullptr);
    xVObj_Basics* baseObjFromId(const QString&);
    void deselectAll();
    void activateNext();
    void dispMemoryConsumption();
    void rDblClickInScene();

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

protected:
    void placeObjInScene(xVObj_Basics* pObj);
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

    xVVisMainWin *pVisMainWin=nullptr;
    bool _inStepMode = false;
    xVObj_Basics* pCurrentStepObj = nullptr;
    long _passCount = 0;
};
#endif // XVTKDLG_H
