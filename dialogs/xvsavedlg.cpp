#include "xvsavedlg.h"
#include "ui_xvsavedlg.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QDateTimeEdit>

xVSaveDlg::xVSaveDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVSaveDlg)
{
    ui->setupUi(this);
    connect(ui->pBrowseTB,SIGNAL(clicked()),this,SLOT(browseFolder()));
}

void xVSaveDlg::browseFolder()
{
    _path=QFileDialog::getExistingDirectory(0,"pick save path");
    if (!_path.isEmpty())
    {
        QFileInfo info(_path);
        ui->pFolderLEdit->setText(info.baseName());
    }
}

bool xVSaveDlg::saveExplicit()
{
    return ui->pIncludeDataCB->isChecked();
}

xVSaveDlg::~xVSaveDlg()
{
    delete ui;
}

void xVSaveDlg::accept()
{
    // saving
    QDialog::accept();
}

void xVSaveDlg::setPixmap(const QPixmap &pix)
{
    QPixmap p(pix);
    p=p.scaledToWidth(359,Qt::SmoothTransformation);
    ui->pPixLab->setPixmap(p);
    ui->pPixLab->setScaledContents(false);
}

QStringList xVSaveDlg::header()
{
    QStringList lst;
    lst << ui->pCreatedByLEdit->text();
    lst << ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm");
    lst << ui->pDescLEdit->text();
    return lst;
}
