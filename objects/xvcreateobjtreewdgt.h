#ifndef XVCREATEOBJTREEWDGT_H
#define XVCREATEOBJTREEWDGT_H

#include <QTreeWidget>
#include <QObject>
#include "xVTypes.h"

class xVCreateObjTreeWdgt : public QTreeWidget
{
    Q_OBJECT
public:
    xVCreateObjTreeWdgt(QWidget* parent=nullptr);

protected slots:
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);
    void itemDoubleClicked_SLOT(QTreeWidgetItem *, int );

protected:
    void generateHooksFromLst(HOOKLstPtr);
    QTreeWidgetItem* pickOrCreateParentItem(const QString&);

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

};

#endif // XVCREATEOBJTREEWDGT_H
