#include "xvpropwdgt.h"
#include "ui_xvpropwdgt.h"
#include <QVariant>
#include "xVCustomTableItems.h"
#include "xVTypes.h"
#include <QScrollBar>
#include <QMessageBox>
#include "xVVolumeVisPropObj.h"

xVPropWdgt::xVPropWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVPropWdgt)
{
    ui->setupUi(this);
    connect(ui->pDeleteTB,SIGNAL(clicked()),this,SLOT(deleteObj()));
    connect(ui->pPreviewTB,SIGNAL(clicked()),this,SLOT(displayPreview()));
    connect(ui->pPropTable,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SIGNAL(KSignal(const SIG_TYPE&,void *)));
}

void xVPropWdgt::installEventFilter(QObject *obj)
{
    ui->pPropTable->installEventFilter(obj);
    QObject::installEventFilter(obj);
}

void xVPropWdgt::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type) {
    case ST_OBJECT_REMOVED: if (pCurrentObj==(xVAbstractBaseObj*)pData)
        {
            pCurrentObj=nullptr;
            hide();
        }
        break;
    default:
        // signal not needed
        break;
    }
}

void xVPropWdgt::deleteObj()
{
    if (pCurrentObj && QMessageBox::question(0,"Warning!","Do you really like to destroy this object?")==QMessageBox::Yes)
    {
        emit KSignal(ST_REMOVE_OBJECT,pCurrentObj);
        pCurrentObj=nullptr;
        hide();
    }
}

void xVPropWdgt::displayPreview()
{

}


xVPropWdgt::~xVPropWdgt()
{
    delete ui;
}

void xVPropWdgt::objSelected(xVAbstractBaseObj* pObj)
{

    if (pCurrentObj!=nullptr) pCurrentObj->setParamSelected(false);
    if (pCurrentObj==pObj) {
        pObj->setParamSelected(false);
        hide();
        pCurrentObj=nullptr;
        return;
    }

    updateParameterTable(pObj);
    ui->pDeleteTB->setEnabled(pObj && pObj->type()!=xVOT_START && pObj->type()!=xVOT_END);
    ui->pPreviewTB->setEnabled(pObj && pObj->hasPreview());
}

void xVPropWdgt::parameterModified()
{
    // close external dialogs

    /*
    xAbstractBasisObj* pObj=dynamic_cast<xAbstractBasisObj*>(sender());
    if (pObj && pObj==pCurrentObj)
    {
        int cr=ui->pPropTable->verticalScrollBar()->value();
        updateParameterTable(pCurrentObj);
        ui->pPropTable->verticalScrollBar()->setValue(cr);
    }
    */
}

void xVPropWdgt::updateParameterTable(xVAbstractBaseObj* pObj)
{
    qApp->processEvents();
    pCurrentObj=pObj;
    pObj->setParamSelected(true);
    ui->pPropTable->updateTable(pCurrentObj->paramMap(),pCurrentObj);
    show();
}
