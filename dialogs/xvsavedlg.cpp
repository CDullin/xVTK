#include "xvsavedlg.h"
#include "ui_xvsavedlg.h"
#include "xvsettingswdgt.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QMessageBox>

xVSaveDlg::xVSaveDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVSaveDlg)
{
    ui->setupUi(this);    
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    connect(ui->pBrowseTB,SIGNAL(clicked()),this,SLOT(browseFolder()));
}

void xVSaveDlg::browseFolder()
{
    _path=QFileDialog::getExistingDirectory(0,"pick save path",::_settings["export dashboard path"]._value.value<xFileName>()._fileName);

    QFileInfo info(_path+"/dashboard.dat");

    if (!_path.isEmpty())
    {
        if (!info.exists() ||
            (info.exists() &&
             QMessageBox::question(0,"Warning","The folder already contains a dashboard. If you continue the data will be overwritten.")==QMessageBox::Yes))
        {
            QFileInfo info(_path);
            ::_settings["export dashboard path"]._value = QString(info.absoluteFilePath()+"/..");
            ui->pFolderLEdit->setText(info.absoluteFilePath());
        }
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
