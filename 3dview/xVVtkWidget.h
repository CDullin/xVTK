#ifndef XVVTKWIDGET_H
#define XVVTKWIDGET_H

#include <QVTKWidget.h>
#include "xVDefine3DTransformationToolWdgt.h"
#include <QMenu>

class xVVtkWidget:public QVTKWidget
{
    Q_OBJECT
public:
    xVVtkWidget(QWidget*parent=nullptr);
protected slots:
    void screenShot();
    void objectTree();
    void defineTrafo();
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    QMenu *pMenu = nullptr;
    xVDefine3DTransformationToolWdgt *pTrafoWdgt;
};

#endif // XVVTKWIDGET_H
