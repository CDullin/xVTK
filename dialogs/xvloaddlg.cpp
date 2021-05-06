#include "xvloaddlg.h"
#include "ui_xvloaddlg.h"
#include <QFile>
#include <QFileDialog>
#include <QDirIterator>
#include <QPixmap>
#include <QDataStream>
#include "xvsettingswdgt.h"
#include <QListWidget>
#include <QPainter>
#include <QMessageBox>

xVLoadDlg::xVLoadDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVLoadDlg)
{
    ui->setupUi(this);
    connect(ui->pBrowseTB,SIGNAL(clicked()),this,SLOT(browseFolder()));
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(itemActivated(QListWidgetItem *)));
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(loadItem(QListWidgetItem *)));    
    connect(ui->listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(updateGui()));
    connect(ui->pDeleteTB,SIGNAL(clicked()),this,SLOT(deleteDashboard()));

    if (::_settings.contains("export dashboard path")) browseFolder(::_settings["export dashboard path"]._value.value<xFileName>()._fileName);
    updateGui();
}

void xVLoadDlg::deleteDashboard()
{
    if (ui->listWidget->selectedItems().count()>0)
    {
        QListWidgetItem *pItem=ui->listWidget->selectedItems()[0];
        QFileInfo info=pItem->data(Qt::UserRole).value<QFileInfo>();
        QFile f(info.absoluteFilePath());

        if (QMessageBox::question(0,"Warning",QString("Do you really like to delete %1 and all its data?").arg(info.absolutePath().section("/",-1,-1)))==QMessageBox::Yes)
        {
            QString path=info.absolutePath();
            QDir d(info.absolutePath());
            d.removeRecursively();
            browseFolder(_lastFolder);
        }
    }
}

void xVLoadDlg::updateGui()
{
    if (ui->listWidget->selectedItems().count()>0)
    {
        ui->pLoadTB->setEnabled(true);
        ui->pDeleteTB->setEnabled(true);
    }
    else
    {
        ui->pLoadTB->setEnabled(false);
        ui->pDeleteTB->setEnabled(false);
    }
}

void xVLoadDlg::browseFolder(const QString& fileName )
{
    QString fname=fileName;
    if (fileName.isEmpty())
    {
        QString path;
        if (::_settings.contains("export dashboard path")) path=::_settings["export dashboard path"]._value.value<xFileName>()._fileName;
        fname=QFileDialog::getExistingDirectory(0,"pick folder that contains dash boards",path);
    }
    else
    {
        QFileInfo info(fileName);
        ui->pFolderLEdit->setText(info.absolutePath());
    }
    if (!fname.isEmpty())
    {
        _lastFolder = fileName;

        ui->listWidget->clear();
        QDirIterator di(fname,QStringList() << "dashboard.dat",QDir::Files,QDirIterator::Subdirectories);
        while (di.hasNext())
        {
            QFileInfo info(di.next());
            QString folder=info.absolutePath();
            folder=folder.section("/",-1,-1);
            QListWidgetItem *pItem=new QListWidgetItem(folder);
            pItem->setData(Qt::UserRole,QVariant::fromValue<QFileInfo>(info));
            ui->listWidget->addItem(pItem);
        }

        if(ui->listWidget->count()>0)
        {
            ui->listWidget->setCurrentItem(ui->listWidget->item(0));
            itemActivated(ui->listWidget->item(0));
        }
    }
}

void xVLoadDlg::itemActivated(QListWidgetItem *pItem)
{
    if (pItem && pItem->data(Qt::UserRole).isValid())
    {
        ui->pLoadTB->setEnabled(true);
        ui->pDeleteTB->setEnabled(true);

        QFileInfo info=pItem->data(Qt::UserRole).value<QFileInfo>();
        QFile f(info.absoluteFilePath());
        if (f.open(QFile::ReadOnly))
        {
            _currentDashboard=info.absoluteFilePath();
            QDataStream d(&f);

            QPixmap pix;
            d >> pix;
            QStringList header;
            d >> header;
            pix=pix.scaledToWidth(300,Qt::SmoothTransformation);
            QPainter pain(&pix);
            QFont fo=font();
            fo.setBold(true);
            pain.setFont(fo);
            long y=0;
            for (int i=0;i<2;++i)
            {
                for (QStringList::iterator it=header.begin();it!=header.end();++it) {
                    pain.drawText(10-2*i,20+y*20-2*i,(*it));
                    ++y;
                }
                pain.setPen(QPen(Qt::white));
                y=0;
            }
            if (pix.isNull()) {
                ui->pPixLab->setPixmap(QPixmap());
                ui->pPixLab->setText("no valid data set");
            }
            else ui->pPixLab->setPixmap(pix);

            f.close();
        }
    }
}

void xVLoadDlg::loadItem(QListWidgetItem *pItem)
{
    itemActivated(pItem);
    accept();
}

xVLoadDlg::~xVLoadDlg()
{
    delete ui;
}
