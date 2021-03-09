#ifndef XVPROPTABLE_H
#define XVPROPTABLE_H

#include <QTableWidget>
#include <QObject>

#include "xVTypes.h"

class xVPropTable : public QTableWidget
{
    Q_OBJECT
public:
    xVPropTable(QWidget* parent=nullptr);
    void updateTable(QMap<QString,xPROP_TYPE>*,xAbstractBasisObj* pCurrentObj=nullptr);
    void updateTable(){updateTable(pParamMpRef,pCurrentObj);}
    bool keyExists(const QString&);
    void setObjectColumnStaysHidden(bool b){_objectColumnStaysHidden=b;}
    xParamMap* paramMap(){return pParamMpRef;}

signals:
    void KSignal(const SIG_TYPE& t,void *pData);
    void paramModified(const QString&);

protected slots:
    void itemChanged_SLOT(QTableWidgetItem*);
    void customItemChanged();
    void subgroupChanged();

protected:
    QMap<QString,xPROP_TYPE>* pParamMpRef = nullptr;
    xAbstractBasisObj *pCurrentObj=nullptr;
    bool _objectColumnStaysHidden = false;
};

#endif // XVPROPTABLE_H
