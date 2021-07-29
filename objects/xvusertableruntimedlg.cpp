#include "xvusertableruntimedlg.h"
#include "ui_xvusertableruntimedlg.h"

xVUserTableRuntimeDlg::xVUserTableRuntimeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVUserTableRuntimeDlg)
{
    ui->setupUi(this);
}

xVUserTableRuntimeDlg::~xVUserTableRuntimeDlg()
{
    delete ui;
}

void xVUserTableRuntimeDlg::setMap(xParamMap *m,xVUserTableImportDlgObj* pObj)
{
    ui->pPropTableWdgt->setObjectColumnStaysHidden(true);
    ui->pPropTableWdgt->updateTable(m,pObj,true);
}
void xVUserTableRuntimeDlg::setTitle(const QString& txt)
{
    ui->pTitleLab->setText(txt);
}
