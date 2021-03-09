#ifndef XVPROPWDGT_H
#define XVPROPWDGT_H

#include <QWidget>
#include "xVObjects.h"
#include <QTableWidgetItem>
#include <QFileInfo>
#include <QMetaProperty>

namespace Ui {
class xVPropWdgt;
}

class xVPropWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit xVPropWdgt(QWidget *parent = nullptr);
    ~xVPropWdgt();
    xAbstractBasisObj* activeObj(){return pCurrentObj;}
    void installEventFilter(QObject*);

public slots:
    void objSelected(xAbstractBasisObj*);
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);
    void parameterModified();

protected slots:
    void deleteObj();

protected:
    void updateParameterTable(xAbstractBasisObj* pObj);

signals:
    void KSignal(const SIG_TYPE& t,void *pData);

private:
    Ui::xVPropWdgt *ui;
    xAbstractBasisObj *pCurrentObj=nullptr;
    QTableWidget *pPropTable=nullptr;
};

#endif // XVPROPWDGT_H
