#include "xvpropwdgt.h"
#include "ui_xvpropwdgt.h"
#include <QVariant>
#include "xVCustomTableItems.h"
#include "xVTypes.h"
#include <QScrollBar>
#include "xVVolumeVisPropObj.h"

xVPropWdgt::xVPropWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVPropWdgt)
{
    ui->setupUi(this);
    connect(ui->pDeleteTB,SIGNAL(clicked()),this,SLOT(deleteObj()));
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
    case ST_OBJECT_REMOVED: if (pCurrentObj==(xAbstractBasisObj*)pData)
        {
            pCurrentObj=nullptr;
            parentWidget()->hide();
        }
        break;
    default:
        // signal not needed
        break;
    }
}

void xVPropWdgt::deleteObj()
{
    if (pCurrentObj)
    {
        emit KSignal(ST_REMOVE_OBJECT,pCurrentObj);
        pCurrentObj=nullptr;
        parentWidget()->hide();
    }
}


xVPropWdgt::~xVPropWdgt()
{
    delete ui;
}

void xVPropWdgt::objSelected(xAbstractBasisObj* pObj)
{

    if (pCurrentObj!=nullptr) pCurrentObj->setParamSelected(false);
    if (pCurrentObj==pObj) {
        pObj->setParamSelected(false);
        parentWidget()->hide();
        pCurrentObj=nullptr;
        return;
    }

    updateParameterTable(pObj);
    if (pObj && pObj->type()==xVOT_START)
    {
        ui->pCloneTB->setEnabled(false);
        ui->pDeleteTB->setEnabled(false);
    }
    else
    {
        ui->pCloneTB->setEnabled(true);
        ui->pDeleteTB->setEnabled(true);
    }
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

void xVPropWdgt::updateParameterTable(xAbstractBasisObj* pObj)
{
    parentWidget()->show();
    pCurrentObj=pObj;
    pObj->setParamSelected(true);
    ui->pPropTable->updateTable(pCurrentObj->paramMap(),pCurrentObj);
}
