#include "xVReportWdgtToolBar.h"
#include "ui_xVReportWdgtToolBar.h"
#include <QMessageBox>
#include <QTimer>

xVReportWdgtToolBar::xVReportWdgtToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVReportWdgtToolBar)
{
    ui->setupUi(this);
    connect(ui->pNewTB,SIGNAL(clicked()),this,SLOT(createReport()));
    connect(ui->pLeftTB,SIGNAL(clicked()),this,SLOT(leftReport()));
    connect(ui->pRightTB,SIGNAL(clicked()),this,SLOT(rightReport()));
    connect(ui->pSaveTB,SIGNAL(clicked()),this,SLOT(saveReport()));
    connect(ui->pSavAsTB,SIGNAL(clicked()),this,SLOT(saveAsReport()));
    connect(ui->pClearTB,SIGNAL(clicked()),this,SLOT(clearReport()));
    connect(ui->pCloseTB,SIGNAL(clicked()),this,SLOT(closeReport()));
    connect(ui->pLoadTB,SIGNAL(clicked()),this,SLOT(loadReport()));
    connect(ui->pReportCB,SIGNAL(currentIndexChanged(int)),this,SLOT(activateReport(int)));

    QTimer *pUpdateTimer = new QTimer();
    pUpdateTimer->setInterval(500);
    pUpdateTimer->setSingleShot(false);
    connect(pUpdateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTimeOut()));
    pUpdateTimer->start();

    updateLimits();
}

void xVReportWdgtToolBar::updateTimerTimeOut()
{
    if (pRef && ui->pReportCB->currentIndex()!=-1)
    {
        if (::_settings["report auto save"]._value.toBool())
        {
            QDateTime dt=QDateTime::currentDateTime();
            long s = -dt.secsTo(pRef->at(ui->pReportCB->currentIndex())->_lastAutoSaved);
            if (s>::_settings["report auto save interval [s]"]._value.value<xLimitedInt>()._value)
            {
                pRef->at(ui->pReportCB->currentIndex())->autoSave();
            }
        }

        if (pRef->at(ui->pReportCB->currentIndex())->_modified)
        {
            QDateTime dt=QDateTime::currentDateTime();
            long s = -dt.secsTo(pRef->at(ui->pReportCB->currentIndex())->_lastModified);
            QString str;
            if (s<60) str=QString("modified, last saved %1s ago").arg(s);
            else
            {
                if (s<3600) str=QString("modified, last saved %1min ago").arg(s/60);
                else
                    str=QString("modified, last saved %1h ago").arg(s/3600);
            }
            ui->pInfoLab->setText(str);
        }
        else
            ui->pInfoLab->setText("up to date");
    }
}

void xVReportWdgtToolBar::createReport()
{
    QString name="untitled";
    int id=0;
    bool found=true;
    while (found)
    {
        found = false;
        for (int i=0;i<ui->pReportCB->count();++i)
            if (ui->pReportCB->itemText(i)==name)
                found = true;
        if (found)
            name=QString("untitled_%1").arg(id++);
    }
    emit KSignal(ST_CREATE_REPORT,new QString(name));

    updateLimits();
}
void xVReportWdgtToolBar::leftReport()
{
    ui->pReportCB->setCurrentIndex(std::max(0,ui->pReportCB->currentIndex()-1));
    updateLimits();
}
void xVReportWdgtToolBar::rightReport()
{
    ui->pReportCB->setCurrentIndex(std::min(ui->pReportCB->count()-1,ui->pReportCB->currentIndex()+1));
    updateLimits();
}
void xVReportWdgtToolBar::saveReport()
{
    emit KSignal(ST_SAVE_CURRENT_REPORT);
}
void xVReportWdgtToolBar::saveAsReport()
{
    emit KSignal(ST_SAVEAS_CURRENT_REPORT);
}
void xVReportWdgtToolBar::clearReport()
{
    if (pRef && QMessageBox::question(0,"Warning","Do you really like to clear the report?")==QMessageBox::Yes)
        (*pRef)[ui->pReportCB->currentIndex()]->clear();
    updateLimits();
}
void xVReportWdgtToolBar::closeReport()
{
    emit KSignal(ST_CLOSE_REPORT,(*pRef)[ui->pReportCB->currentIndex()]);
    updateLimits();
}
void xVReportWdgtToolBar::loadReport()
{
    emit KSignal(ST_LOAD_REPORT);
    updateLimits();
}
void xVReportWdgtToolBar::activateReport(int id)
{
    if (pRef && id>-1 && id<pRef->count())
        emit KSignal(ST_ACTIVATE_REPORT,new QString((*pRef)[id]->_name));
}

xVReportWdgtToolBar::~xVReportWdgtToolBar()
{
    delete ui;
}

void xVReportWdgtToolBar::setReportMapRef(QList <xVReport*>* pL)
{
    pRef=pL;
    updateToolbar();
}

void xVReportWdgtToolBar::updateToolbar()
{
    if (pRef)
    {
        ui->pInfoLab->setText("");
        QStringList items;
        for (int i=0;i<pRef->count();++i)
            items << pRef->at(i)->_name;
        ui->pReportCB->clear();
        ui->pReportCB->insertItems(0,items);
    }
    updateLimits();
}

void xVReportWdgtToolBar::updateLimits()
{
    ui->pLeftTB->setEnabled(ui->pReportCB->count()>0 && ui->pReportCB->currentIndex()>0);
    ui->pRightTB->setEnabled(ui->pReportCB->count()>0 && ui->pReportCB->currentIndex()<(ui->pReportCB->count()-1));
    ui->pClearTB->setEnabled(ui->pReportCB->count()>0);
    ui->pCloseTB->setEnabled(ui->pReportCB->count()>0);
    ui->pSaveTB->setEnabled(ui->pReportCB->count()>0);
    ui->pSavAsTB->setEnabled(ui->pReportCB->count()>0);
}

void xVReportWdgtToolBar::KSlot(const SIG_TYPE& type,void* pData)
{
    switch (type)
    {
    case ST_REPORT_CREATED:
        updateToolbar();
        break;
    case ST_REPORT_ACTIVATED:
        if (pRef) ui->pReportCB->setCurrentIndex(pRef->indexOf((xVReport*)pData));
        break;
    default:
        // handled
        break;
    }
}
