#ifndef XVEQUATIONDLGITEM_H
#define XVEQUATIONDLGITEM_H

#include <QObject>
#include <QToolButton>
#include "xVObjects.h"
#include "xVTypes.h"
#include "xVObjectTypes.h"
#include "xvEvalCondition.h"

class xVEquationDlgItem : public QToolButton
{
    Q_OBJECT
public:
    xVEquationDlgItem(xVEvalCondition pEP,xVObj_Basics* pRefObj,QWidget* parent=nullptr);
    QString equation();
signals:
    void modified();
    void KSignal(const SIG_TYPE& ,void *);
protected slots:
    void clicked_SLOT();
protected:
    QString _equation;
    xVObj_Basics* pRefObj=nullptr;
};


#endif // XVEQUATIONDLGITEM_H
