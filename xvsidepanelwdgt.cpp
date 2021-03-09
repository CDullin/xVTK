#include "xvsidepanelwdgt.h"
#include <QLabel>
#include <QHeaderView>
#include "xvtkdlg.h"

xVSidePanelWdgt::xVSidePanelWdgt(QWidget *parent) : QWidget(parent)
{
    setGeometry(0,0,120,500);
    setMinimumWidth(120);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    pBoxLayout = new QVBoxLayout(this);
    pBoxLayout->setMargin(0);
    pBoxLayout->setGeometry(QRect(0,0,120,500));
    pBoxLayout->addWidget(new QLabel("objects"));
    pTreeWdgt = new xVCreateObjTreeWdgt(this);
    pTreeWdgt->header()->setVisible(false);
    pBoxLayout->addWidget(pTreeWdgt);
}

void xVSidePanelWdgt::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type) {
    case ST_GENERATE_CONNECTION_WITH_MAIN_DLG:
    {
        // connect the children
        xVTKDlg* pMainDlg=(xVTKDlg*)pData;
        connect(pTreeWdgt,SIGNAL(KSignal(const SIG_TYPE&,void *)),pMainDlg,SLOT(KSlot(const SIG_TYPE&,void *)));
        connect(pMainDlg,SIGNAL(KSignal(const SIG_TYPE&,void *)),pTreeWdgt,SLOT(KSlot(const SIG_TYPE&,void *)));
        pTreeWdgt->installEventFilter(pMainDlg);
    }
        break;
    default:
        // no need to handle everything
        break;
    }

}
