#include "xVCVSImportPreviewDlg.h"
#include "ui_xVCVSImportPreviewDlg.h"
#include "xVImportCVSObj.h"
#include "xVAbstractBaseObj.h"
#include "xVObjects.h"

#include <QTextStream>

xVCVSImportPreviewDlg::xVCVSImportPreviewDlg(xVImportCVSObj *p,xVTable* pTab,QWidget *parent) :
    QDialog(parent),
    pTabRef(pTab),
    ui(new Ui::xVCVSImportPreviewDlg)
{
    pCVSObj=p;
    _paramMp["separator"]._id=1;
    _paramMp["separator"]._value=(*(pCVSObj->paramMap()))["separator"]._value;
    _paramMp["header skip [lines]"]._id=2;
    _paramMp["header skip [lines]"]._value=(*(pCVSObj->paramMap()))["header skip [lines]"]._value;
    _paramMp["skip every N [line]"]._id=5;
    _paramMp["skip every N [line]"]._value=(*(pCVSObj->paramMap()))["skip every N [line]"]._value;
    _paramMp["skip every N [column]"]._id=6;
    _paramMp["skip every N [column]"]._value=(*(pCVSObj->paramMap()))["skip every N [column]"]._value;
    _paramMp["data ordered in columns"]._id=7;
    _paramMp["data ordered in columns"]._value=(*(pCVSObj->paramMap()))["data ordered in columns"]._value;
    _paramMp["autodetect data type"]._id=8;
    _paramMp["autodetect data type"]._value=(*(pCVSObj->paramMap()))["autodetect data type"]._value;
    _paramMp["use first N lines as label"]._id=9;
    _paramMp["use first N lines as label"]._value=(*(pCVSObj->paramMap()))["use first N lines as label"]._value;
    _paramMp["decimal point"]._id=10;
    _paramMp["decimal point"]._value=(*(pCVSObj->paramMap()))["decimal point"]._value;

    ui->setupUi(this);
    ui->pPropTable->updateTable(&_paramMp);
    ui->pFileContentBrowser->setWordWrapMode(QTextOption::NoWrap);
    //connect(ui->pReloadFileTB,SIGNAL(clicked()),this,SLOT(loadAndDisplayFile()));
    //connect(ui->pUpdateTableTB,SIGNAL(clicked()),this,SLOT(updateAndDisplayTable()));
}

xVCVSImportPreviewDlg::~xVCVSImportPreviewDlg()
{
    delete ui;
}

void xVCVSImportPreviewDlg::loadAndDisplayFile()
{
    if (!pTabRef || pTabRef->file().isEmpty()) return;
    QFile f(pTabRef->file());
    if (f.open(QFile::ReadOnly))
    {
        ui->pFileContentBrowser->clear();
        QTextStream t(&f);
        while (!t.atEnd())
        {
            ui->pFileContentBrowser->append(t.readLine());
        }

        f.close();
    }
}

void xVCVSImportPreviewDlg::updateAndDisplayTable()
{
    if (!pTabRef) return;
    ui->pDataTable->setUpdatesEnabled(false);
    ui->pDataTable->clearContents();
    ui->pDataTable->setRowCount(pTabRef->rowCount());
    ui->pDataTable->setColumnCount(pTabRef->colCount());
    ui->pDataTable->verticalHeader()->setVisible(false);
    QStringList header;
    for (int h=0;h<pTabRef->header()->count();++h) {
        header << pTabRef->header()->at(h).join("\n");
    }
    ui->pDataTable->setHorizontalHeaderLabels(header);
    for (long r=0;r<pTabRef->rowCount();++r)
        for (long c=0; c<pTabRef->colCount();++c) {
            ui->pDataTable->setItem(r,c,new QTableWidgetItem(pTabRef->item(r,c).toString()));
        }
    ui->pDataTable->setUpdatesEnabled(true);
    ui->pDataTable->update();
}

void xVCVSImportPreviewDlg::updateDialog()
{
    loadAndDisplayFile();
    updateAndDisplayTable();
}
