#include "xVAxisDefinitionDlg.h"
#include "ui_xVAxisDefinitionDlg.h"

xVAxisDefinitionDlg::xVAxisDefinitionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVAxisDefinitionDlg)
{
    ui->setupUi(this);
    connect(ui->pPropTable,SIGNAL(paramModified(const QString&)),this,SLOT(displayFont()));
}

xVAxisDefinitionDlg::~xVAxisDefinitionDlg()
{
    delete ui;
}

void xVAxisDefinitionDlg::setParameterRef(xParamMap *pRef)
{
    pParamMpRef = pRef;
    pCopyParamMp = new xParamMap(*pRef);
    // fill in gui
    ui->pPropTable->updateTable(pParamMpRef);
}

void xVAxisDefinitionDlg::accept()
{
    // copy values from gui to parameter map
    QDialog::accept();
}

void xVAxisDefinitionDlg::reject()
{
    pParamMpRef = pCopyParamMp;
    QDialog::reject();
}

void xVAxisDefinitionDlg::KSlot(const SIG_TYPE& _type,void *pData)
{

}
