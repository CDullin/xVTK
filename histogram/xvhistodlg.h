#ifndef XVHISTODLG_H
#define XVHISTODLG_H

#include <QDialog>
#include "xVHisto.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QObject>
#include <QGraphicsView>
#include <QLabel>
#include <QGraphicsPixmapItem>

#include "xVHistoDlg_types.h"
#include "xVHistoNodeItem.h"
#include "xVHistoHValueAxis.h"
#include "xVHistoPixmapItem.h"
#include "xVHistoVLine.h"
#include "xVHistoVValueAxis.h"
#include "xvproptable.h"

class xVHistoDlg : public QDialog
{
    Q_OBJECT
public:

    enum HISTO_DLG_MODE
    {
        HDM_WINDOW_LEVEL    =0x00,
        HDM_OPACITY         =0x01,
        HDM_LUT             =0x02,
        HDM_MEASURE         =0x03
    };

    enum HISTO_LINE_MODE
    {
        HLM_ZOOM_LEFT,
        HLM_ZOOM_RIGHT,
        HLM_HOR_AXIS_ALL,
        HLM_VER_AXIS_ALL,
        HLM_HOR_AXIS_ZOOM,
        HLM_VER_AXIS_ZOOM
    };

    explicit xVHistoDlg(QWidget *parent = nullptr);
    ~xVHistoDlg();
    xVHISTO_PIXMAP generatePixmap(const float& _dMin,const float& _dMax,const int& w,const int& h);
    void setMode(const HISTO_DLG_MODE& m);
    void setHisto(xVHisto* h){pData=h;}
    void setOpacityFunc(vtkPiecewiseFunctionPtr f);
    void setColorTransFunc(vtkColorTransferFunctionPtr f);

    float mapGValToAllPixmap(float,bool&);
    float mapGValToZoomPixmap(float,bool&);
    QPointF mapPxFrmAllPixmapToGValAndIntensity(QPointF,bool&);
    QPointF mapPxFrmZoomPixmapToGValAndIntensity(QPointF,bool&);

    static QPixmap bar(const int& mode,const int& w,const int& h,const double& miV,const double& maV,void* data);

    void accept() override;
    void reject() override;

protected slots:
    void dispCursorPos(QPointF);
    void interactiveElementMoved(QPointF,const int&);
    void interactiveElementDblClicked(QPointF, const int&);
    void interactiveLEditTriggered();
    void nodeMoved(QPointF,const int&,xVHistoNodeItem*);
    void dblClickInAll(QPointF);
    void dblClickInZoom(QPointF);
    void killNode(xVHistoNodeItem*);
    void nodeColorChanged(xVHistoNodeItem*);
    void dblClickInColorBar(QPointF);
    void dblClickInZoomColorBar(QPointF);
    void paramModified_SLOT(const QString&);
    void saveSettings();
    void loadSettings();

signals:
    void modified(const QString&);
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

protected:
    void createOpacityFunc();
    void updateOpacityFunc();
    void updateView();
    QPixmap generateColorBar(int w,int h,bool zoom=false);

    void createColorFunc();
    void updateColorFunc();

    virtual void resizeEvent(QResizeEvent *event) override;
    xVHISTO_PIXMAP _all,_zoom;
    xVHistoPixmapItem *pAllHistoPixItem = nullptr;
    xVHistoPixmapItem *pZoomHistoPixItem = nullptr;
    xVHistoPixmapItem *pColorBarPixItem = nullptr;
    xVHistoPixmapItem *pZoomColorBarPixItem = nullptr;
    HISTO_DLG_MODE _mode=HDM_OPACITY;
    xVHisto* pData=nullptr;
    QVBoxLayout *pVBLayout;
    QHBoxLayout *pHBLayout;
    QLineEdit *pLEdit;
    QToolButton *pCancelTB,*pApplyTB;
    QGraphicsView *pHistoGV;
    QLabel *pStatusLab;
    xVHistoVLine *pZoomLeft = nullptr,*pZoomRight = nullptr;
    QGraphicsLineItem *pZoomConLeft = nullptr;
    QGraphicsLineItem *pZoomConRight = nullptr;
    QLineEdit *pInteractiveLEdit = nullptr;
    HISTO_LINE_MODE _currentInteractiveObj;
    xVHistoHValueAxis *pHorAllAxis=nullptr;
    xVHistoHValueAxis *pHorZoomAxis=nullptr;
    xVHistoVValueAxis *pVerAllAxis=nullptr;
    xVHistoVValueAxis *pVerZoomAxis=nullptr;

    vtkPiecewiseFunctionPtr opacityFuncRef = nullptr;
    vtkPiecewiseFunctionPtr cpyOpacityFuncRef = nullptr;
    QList <xVHistoNodeItem*> _opacityNodeLst;
    QList <xVHistoNodeItem*> _opacityZoomNodeLst;
    QGraphicsPathItem *pOpacityPathItem = nullptr;
    QGraphicsPathItem *pZoomOpacityPathItem = nullptr;

    vtkColorTransferFunctionPtr colorTransFuncRef = nullptr;
    vtkColorTransferFunctionPtr cpyColorTransFuncRef = nullptr;
    QList <xVHistoNodeItem*> _colorNodeLst;
    QList <xVHistoNodeItem*> _colorZoomNodeLst;

    QLabel* pValueLabel;
    QMap<QString,xPROP_TYPE> _paramMp;
    xVPropTable *pPropTableWdgt = nullptr;
};

#endif // XVHISTODLG_H
