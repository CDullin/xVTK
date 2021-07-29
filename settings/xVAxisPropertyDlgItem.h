#ifndef XVAXISPROPERTYDLGITEM_H
#define XVAXISPROPERTYDLGITEM_H

#include "xVTypes.h"
#include <vtkAxisActor2D.h>
#include "xVTextPropertyDlgItem.h"
#include <QToolButton>

class xVAxisProp
{
public:
    xParamMap _paramMp;
    vtkAxisActor2D *pVtkAxisProp = nullptr;
    xVAxisProp();
    ~xVAxisProp();
    xVAxisProp(const xVAxisProp& other);
    vtkAxisActor2D* generateVtkAxisPropFromParam();
    void generateParamFromVtkAxisProp(vtkAxisActor2D *pProp);
};

typedef xVAxisProp* xVAxisPropPtr;
Q_DECLARE_METATYPE(xVAxisPropPtr);

QDataStream &operator<<(QDataStream &out, const xVAxisPropPtr &myObj);
QDataStream &operator>>(QDataStream &in, xVAxisPropPtr &myObj);

class xVAxisPropertyDlgItem: public QToolButton
{
    Q_OBJECT
public:
    xVAxisPropertyDlgItem(QWidget* parent=nullptr);
    void setRef(xVAxisProp*);
signals:
    void modified();
    void KSignal(const SIG_TYPE& ,void *);
protected slots:
    void clicked_SLOT();
protected:
    xVAxisProp* pRefObj=nullptr;
};


#endif // XVAXISPROPERTYDLGITEM_H
