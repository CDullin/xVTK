#include "xVCVSImportPreviewDlg.h"
#include "ui_xVCVSImportPreviewDlg.h"

xVCVSImportPreviewDlg::xVCVSImportPreviewDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVCVSImportPreviewDlg)
{
    ui->setupUi(this);
}

xVCVSImportPreviewDlg::~xVCVSImportPreviewDlg()
{
    delete ui;
}
