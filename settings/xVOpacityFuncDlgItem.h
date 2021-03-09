#ifndef XVOPACITYFUNCDLGITEM_H
#define XVOPACITYFUNCDLGITEM_H

#include <QLabel>
#include "xVTypes.h"

class xVVolumeVisPropObj;

class xVOpacityFuncDlgItem:public QLabel
{
Q_OBJECT
public:
    xVOpacityFuncDlgItem(QWidget *parent = nullptr);
    void setOpcityFunc(vtkPiecewiseFunctionPtr pORef);
    void setRefVolumeObj(xVVolumeVisPropObj* pRObj);
signals:
    void modified();
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

    void generatePixmap();

    vtkPiecewiseFunctionPtr pOpacityFunc=nullptr;
    xVVolumeVisPropObj* pVolVisPropObj=nullptr;
};
#endif // XVOPACITYFUNCDLGITEM_H
