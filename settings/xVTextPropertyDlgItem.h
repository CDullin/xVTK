#ifndef XVTEXTPROPERTYDLGITEM_H
#define XVTEXTPROPERTYDLGITEM_H

#include "xVTypes.h"
#include <QToolButton>

class xVTextProp
{
public:
    xParamMap _paramMp;
    vtkTextProperty *pVtkTextProp = nullptr;
    xVTextProp();
    ~xVTextProp();
    xVTextProp(const xVTextProp& other);
    vtkTextProperty* generateVtkTextPropFromParam();
    QFont generateQtFontFromParam();
    void generateParamFromVtkTextProp(vtkTextProperty *pProp);
};

typedef xVTextProp* xVTextPropPtr;
Q_DECLARE_METATYPE(xVTextPropPtr);

QDataStream &operator<<(QDataStream &out, const xVTextPropPtr &myObj);
QDataStream &operator>>(QDataStream &in, xVTextPropPtr &myObj);

class xVTextPropertyDlgItem: public QToolButton
{
    Q_OBJECT
public:
    xVTextPropertyDlgItem(QWidget* parent=nullptr);
    void setRef(xVTextProp*);
signals:
    void modified();
    void KSignal(const SIG_TYPE& ,void *);
protected slots:
    void clicked_SLOT();
protected:
    xVTextProp* pRefObj=nullptr;
};


#endif // XVTEXTPROPERTYDLGITEM_H
