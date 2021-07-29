#include "xVSessionDlg.h"
#include "ui_xVSessionDlg.h"
#include "xviconfactory.h"
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QBuffer>

xVSessionDlg::xVSessionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVSessionDlg)
{
    ui->setupUi(this);
    connect(ui->pSessionTreeWdgt,SIGNAL(itemClicked(QTreeWidgetItem *, int )),this,SLOT(itemActivated(QTreeWidgetItem *, int )));
    connect(ui->pDeleteTB,SIGNAL(clicked()),this,SLOT(deleteSelectedSession()));
    connect(ui->pLoadTB,SIGNAL(clicked()),this,SLOT(loadCurrentItem()));
}

void xVSessionDlg::itemActivated(QTreeWidgetItem *item, int column)
{
    if (item!=nullptr)
    {
        ui->pDeleteTB->setEnabled(true);
        ui->pLoadTB->setEnabled(true);
    }
    else
    {
        ui->pDeleteTB->setEnabled(false);
        ui->pLoadTB->setEnabled(false);
    }
}

xVSessionDlg::~xVSessionDlg()
{
    // clean up
    removeCurrentTmpFolder();

    if (QMessageBox::question(0,"Warning","Do you like to store the current session?")==QMessageBox::Yes)
    {
        saveSession();
    }
    delete ui;
}

int xVSessionDlg::exec()
{
    int _returnCode=0;
    // check if a session was found
    updateGui();
    if (ui->pSessionTreeWdgt->topLevelItemCount()>0)
        _returnCode = QDialog::exec();
        //show();
    else
        emit KSignal(ST_MSG,new QString("No stored sessions found"));
    return _returnCode;
}

void xVSessionDlg::addDashboards(QTreeWidgetItem* pParentItem,QString folder)
{
    QDir d(folder+"/dashboards");
    d.setFilter(QDir::Dirs);
    if (d.exists())
    {
        QFileInfoList lst=d.entryInfoList();
        for (QFileInfoList::iterator it=lst.begin();it!=lst.end();++it)
        {
            if (it->isDir() && it->fileName()!="." && it->fileName()!="..")
            {
                QDir fd(it->absoluteFilePath());
                fd.setFilter(QDir::Files);
                fd.setNameFilters(QStringList("dashboard_*.dat"));
                QFileInfoList fdList=fd.entryInfoList();
                if (fdList.count()>0)
                {
                    QTreeWidgetItem *pItem = new QTreeWidgetItem(pParentItem,QStringList() << "db_"+it->baseName());
                    pItem->setIcon(0,_iconFactory.icon("dashboard"));
                    pItem->setData(1,Qt::UserRole,QVariant(fdList[0].absoluteFilePath()));
                    pItem->setData(2,Qt::UserRole,QVariant(1));
                    pItem->setData(3,Qt::UserRole,QVariant(false));
                }
            }
        }
    }
}

void xVSessionDlg::addReports(QTreeWidgetItem* pParentItem,QString folder)
{
    QDir d(folder+"/reports");
    d.setFilter(QDir::Files);
    d.setNameFilters(QStringList()<<"*.html");
    if (d.exists())
    {
        QFileInfoList lst=d.entryInfoList();
        for (QFileInfoList::iterator it=lst.begin();it!=lst.end();++it)
        {
            QTreeWidgetItem *pItem = new QTreeWidgetItem(pParentItem,QStringList() << "rp_"+it->baseName());
            pItem->setIcon(0,_iconFactory.icon("report"));
            pItem->setData(1,Qt::UserRole,QVariant(it->absoluteFilePath()));
            pItem->setData(2,Qt::UserRole,QVariant(2));
            pItem->setData(3,Qt::UserRole,QVariant(false));
        }
    }
}

void xVSessionDlg::updateGui()
{
    ui->pSessionTreeWdgt->clear();
    // scan temp folder
    QDir d(::_settings["temp folder"]._value.value<xFileName>()._fileName);
    QFileInfoList lst=d.entryInfoList(QDir::Dirs);
    for (QFileInfoList::iterator it=lst.begin();it!=lst.end();++it)
    {
        if (it->isDir() && it->fileName()!="." && it->fileName()!=".." && it->fileName()!=_session)
        {
            QTreeWidgetItem *pItem = new QTreeWidgetItem(ui->pSessionTreeWdgt,QStringList() << it->baseName());
            pItem->setIcon(0,_iconFactory.icon("session"));
            pItem->setData(1,Qt::UserRole,QVariant(it->absoluteFilePath()));
            pItem->setData(2,Qt::UserRole,QVariant(0));
            pItem->setData(3,Qt::UserRole,QVariant(false));
            addDashboards(pItem,it->absoluteFilePath());
            addReports(pItem,it->absoluteFilePath());
        }
    }
}

void xVSessionDlg::accept()
{
    loadCurrentItem();
    QDialog::accept();
}

void xVSessionDlg::reject()
{
    if (ui->pClearAllIncompleteSessionsCB->isChecked())
        deleteAllIncompleteSessions();
    QDialog::reject();
}

void xVSessionDlg::deleteSelectedSession()
{
    if (ui->pSessionTreeWdgt->selectedItems().count()==0) return;
    QTreeWidgetItem *pItem = ui->pSessionTreeWdgt->selectedItems()[0];
    while (pItem->parent()!=0) {
        pItem=pItem->parent();
    }
    if (QMessageBox::question(0,"Warning",QString("Do you really like to delete session %1 with all its content?").arg(pItem->text(0)))==QMessageBox::Yes)
    {
        QDir d(pItem->data(1,Qt::UserRole).toString());
        if (!d.removeRecursively())
            emit KSignal(ST_ERROR_MSG,new QString(QString("Could not delete %1").arg(d.path())));
    }
    updateGui();
}

void xVSessionDlg::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type)
    {
    default:
        // handled
        break;
    }
}

void xVSessionDlg::removeCurrentTmpFolder()
{
    QString fname = ::_settings["temp folder"]._value.value<xFileName>()._fileName+"/"+::_session;
    QDir d(fname);
    if (!d.removeRecursively())
        emit KSignal(ST_ERROR_MSG,new QString("Unable to remove temp folder"));
}
void xVSessionDlg::deleteAllIncompleteSessions()
{
    for (int i=0; i<ui->pSessionTreeWdgt->topLevelItemCount(); ++i) {
        QTreeWidgetItem *pItem = ui->pSessionTreeWdgt->topLevelItem(i);
        if (pItem->text(0).startsWith("xVTK_session_"))
        {
            QDir d(pItem->data(1,Qt::UserRole).toString());
            if (!d.removeRecursively())
                emit KSignal(ST_ERROR_MSG,new QString(QString("Could not delete %1").arg(d.path())));
        }
    }
}
void xVSessionDlg::loadCurrentItem()
{
    if (ui->pSessionTreeWdgt->selectedItems().count()==0) return;
    QTreeWidgetItem *pItem = ui->pSessionTreeWdgt->selectedItems()[0];
    if (pItem->data(3,Qt::UserRole).toBool()==false)
    {
        switch (pItem->data(2,Qt::UserRole).toInt())
        {
        case 0 : loadSession(pItem);break;
        case 1 : loadDashboard(pItem->data(1,Qt::UserRole).toString());break;
        case 2 : loadReport(pItem->data(1,Qt::UserRole).toString());break;
        }
        pItem->setData(3,Qt::UserRole,QVariant(true));
    }
    else emit KSignal(ST_ERROR_MSG,new QString(QString("%1 has already been loaded").arg(pItem->text(0))));
}
void xVSessionDlg::loadSession(QTreeWidgetItem* pItem)
{
    for (int i=0; i<pItem->childCount(); ++i) {
        QTreeWidgetItem *pChild = pItem->child(i);
        switch (pChild->data(2,Qt::UserRole).toInt())
        {
        case 1 : loadDashboard(pChild->data(1,Qt::UserRole).toString());break;
        case 2 : loadReport(pChild->data(1,Qt::UserRole).toString());break;
        }
    }
}
void xVSessionDlg::loadDashboard(const QString& fname)
{
    emit KSignal(ST_LOAD_DASHBOARD, new QString(fname));
}
void xVSessionDlg::loadReport(const QString& fname)
{
    emit KSignal(ST_LOAD_REPORT, new QString(fname));
}
void xVSessionDlg::saveSession()
{
    QString path = QFileDialog::getExistingDirectory(0,"Save Session in empty path",::_settings["temp folder"]._value.value<xFileName>()._fileName);
    if (!path.isEmpty())
    {
        QDir d(path);
        d.setFilter(QDir::Dirs|QDir::Files);
        if (d.entryInfoList().count()<=2)
        {
            QString _currentPath = QDir::currentPath();
            d.setCurrent(path);
            // save all dashboards
            int id = _currentDashBoard;
            for (int i=0; i<::_dashboardLst.count(); ++i) {
                d.mkpath(path+"/dashboards/"+_dashboardLst[i]->_name);
                _currentDashBoard = i;
                QString oldFname = _dashboardLst[i]->_fileName;
                bool bound = _dashboardLst[i]->_boundToRealFileName;

                _dashboardLst[i]->_fileName = path+"/dashboards/"+_dashboardLst[i]->_name;
                _dashboardLst[i]->_boundToRealFileName = true;
                emit KSignal(ST_SAVE_DASHBOARD);

                _dashboardLst[i]->_fileName = oldFname;
                _dashboardLst[i]->_boundToRealFileName = bound;
            }
            _currentDashBoard = id;

            //save all reports
            d.mkpath(path+"/reports");
            emit KSignal(ST_SAVE_ALL_REPORTS,new QString(path+"/reports"));
        }
        else
            emit KSignal(ST_ERROR_MSG,new QString("Path is not empty"));
    }
}

