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

xVLoadDlg::xVLoadDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVLoadDlg)
{
    ui->setupUi(this);
    connect(ui->pBrowseTB,SIGNAL(clicked()),this,SLOT(browseFolder()));
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(itemActivated(QListWidgetItem *)));

    if (::_settings.contains("export dashboard path")) browseFolder(::_settings["export dashboard path"]._value.toString());
}

void xVLoadDlg::browseFolder(const QString& fileName )
{
    QString fname=fileName;
    if (fileName.isEmpty())
    {
        QString path;
        if (::_settings.contains("export dashboard path")) path=::_settings["export dashboard path"]._value.toString();
        fname=QFileDialog::getExistingDirectory(0,"pick folder that contains dash boards",path);
    }
    else
    {
        QFileInfo info(fileName);
        ui->pFolderLEdit->setText(info.absolutePath());
    }
    if (!fname.isEmpty())
    {
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

xVLoadDlg::~xVLoadDlg()
{
    delete ui;
}
