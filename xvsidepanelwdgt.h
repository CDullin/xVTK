#ifndef XVSIDEPANELWDGT_H
#define XVSIDEPANELWDGT_H

#include <QWidget>
#include <QVBoxLayout>
#include "xVTypes.h"
#include "xvcreateobjtreewdgt.h"

class xVSidePanelWdgt : public QWidget
{
    Q_OBJECT
public:
    explicit xVSidePanelWdgt(QWidget *parent = nullptr);

protected slots:
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

protected:
    xVCreateObjTreeWdgt *pTreeWdgt=nullptr;
    QVBoxLayout *pBoxLayout=nullptr;
};

#endif // XVSIDEPANELWDGT_H
