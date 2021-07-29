#include "xVVtkWidget.h"
#include <QMouseEvent>
#include <QVTKOpenGLWidget.h>
#include <vtkRenderWindow.h>

xVVtkWidget::xVVtkWidget(QWidget*parent):QVTKWidget(parent)
{
    pMenu = new QMenu();
    pMenu->addAction("screen shot",this,SLOT(screenShot()));
    pMenu->addAction("object tree",this,SLOT(objectTree()));
    pMenu->addAction("define transformation",this,SLOT(defineTrafo()));

    pTrafoWdgt = new xVDefine3DTransformationToolWdgt(this);
    pTrafoWdgt->hide();
}

void xVVtkWidget::mouseDoubleClickEvent(QMouseEvent *me)
{
    if (me->button()==Qt::RightButton && !pMenu->isVisible())
        pMenu->popup(me->globalPos());
    else return QVTKWidget::mouseDoubleClickEvent(me);
}

void xVVtkWidget::screenShot()
{
    // screen shot
}

void xVVtkWidget::defineTrafo()
{
    QPoint p=mapFromGlobal(QCursor::pos())-pTrafoWdgt->rect().center();
    if (p.x()<5) p.setX(5);
    if (p.y()<5) p.setY(5);
    if (p.x()+pTrafoWdgt->rect().width()+5>width()) p.setX(width()-pTrafoWdgt->width()-5);
    if (p.y()+pTrafoWdgt->rect().height()+5>height()) p.setY(height()-pTrafoWdgt->height()-5);

    pTrafoWdgt->move(p);
    pTrafoWdgt->raise();
    pTrafoWdgt->show();
}

void xVVtkWidget::objectTree()
{}

