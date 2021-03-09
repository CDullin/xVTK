#ifndef XVLUTFUNCDLGITEM_H
#define XVLUTFUNCDLGITEM_H

#include <QLabel>
#include "xVTypes.h"

class xVVolumeVisPropObj;

class xVLUTFuncDlgItem:public QLabel
{
Q_OBJECT
public:
    xVLUTFuncDlgItem(QWidget *parent = nullptr);
    void setLUTFunc(vtkColorTransferFunctionPtr pORef);
    void setRefVolumeObj(xVVolumeVisPropObj* pRObj);
signals:
    void modified();
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

    void generatePixmap();

    vtkColorTransferFunctionPtr pLutFunc=nullptr;
    xVVolumeVisPropObj* pVolVisPropObj=nullptr;
};

#endif // XVLUTFUNCDLGITEM_H
