#include "xVDashboardToolBar.h"
#include "ui_xVDashboardToolBar.h"
#include <QTimer>

xVDashboardToolBar::xVDashboardToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVDashboardToolBar)
{
    ui->setupUi(this);
    connect(ui->pNewTB,SIGNAL(clicked()),this,SLOT(createDashboard()));
    connect(ui->pLeftTB,SIGNAL(clicked()),this,SLOT(leftDashboard()));
    connect(ui->pRightTB,SIGNAL(clicked()),this,SLOT(rightDashboard()));
    connect(ui->pSaveTB,SIGNAL(clicked()),this,SLOT(saveDashboard()));
    connect(ui->pSavAsTB,SIGNAL(clicked()),this,SLOT(saveAsDashboard()));
    connect(ui->pClearTB,SIGNAL(clicked()),this,SLOT(clearDashboard()));
    connect(ui->pCloseTB,SIGNAL(clicked()),this,SLOT(closeDashboard()));
    connect(ui->pLoadTB,SIGNAL(clicked()),this,SLOT(loadDashboard()));
    connect(ui->pDashboardCB,SIGNAL(currentIndexChanged(int)),this,SLOT(activateDashboard(int)));

    pUpdateTimer = new QTimer();
    pUpdateTimer->setSingleShot(false);
    pUpdateTimer->setInterval(1000);
    connect(pUpdateTimer,SIGNAL(timeout()),this,SLOT(updateStatusLab()));
}

xVDashboardToolBar::~xVDashboardToolBar()
{
    delete ui;
}
void xVDashboardToolBar::closeDashboard()
{
    emit KSignal(ST_CLOSE_DASHBOARD);
}
void xVDashboardToolBar::clearDashboard()
{
    emit KSignal(ST_CLEAR_DASHBOARD);
}
void xVDashboardToolBar::createDashboard()
{
    emit KSignal(ST_CREATE_DASHBOARD);
}
void xVDashboardToolBar::leftDashboard()
{
    ui->pDashboardCB->setCurrentIndex(ui->pDashboardCB->currentIndex()-1);
}
void xVDashboardToolBar::rightDashboard()
{
    ui->pDashboardCB->setCurrentIndex(ui->pDashboardCB->currentIndex()+1);
}
void xVDashboardToolBar::saveDashboard()
{
    emit KSignal(ST_SAVE_DASHBOARD);
}
void xVDashboardToolBar::loadDashboard()
{
    emit KSignal(ST_LOAD_DASHBOARD);
}
void xVDashboardToolBar::saveAsDashboard()
{
    emit KSignal(ST_SAVEAS_DASHBOARD);
}
void xVDashboardToolBar::updateStatusLab()
{
    int i=ui->pDashboardCB->currentIndex();
    if (_dashboardLst.count()>0 && i>-1)
    {
        QDateTime dt=QDateTime::currentDateTime();

        if (::_settings["auto save dashboard"]._value.toBool())
        {
            long s = -dt.secsTo(_dashboardLst[i]->_lastAutoSaved);
            if (s>::_settings["auto save dashboard interval [s]"]._value.value<xLimitedInt>()._value)
            {
                emit KSignal(ST_AUTOSAVE_DASHBOARD,_dashboardLst[i]);
            }
        }


        long s = -dt.secsTo(_dashboardLst[i]->_lastSaved);
        QString str;
        if (s<60) str=QString("last saved %1s ago").arg(s);
        else
        {
            if (s<3600) str=QString("last saved %1min ago").arg(s/60);
            else
                str=QString("last saved %1h ago").arg(s/3600);
        }
        ui->pInfoLab->setText(str);
    }
}
void xVDashboardToolBar::activateDashboard(int i)
{
    emit KSignal(ST_ACTIVATE_DASHBOARD,new int(i));
    updateGui();
}
void xVDashboardToolBar::updateGui()
{
    disconnect(ui->pClearTB,SIGNAL(clicked()),this,SLOT(clearDashboard()));
    disconnect(ui->pCloseTB,SIGNAL(clicked()),this,SLOT(closeDashboard()));
    disconnect(ui->pNewTB,SIGNAL(clicked()),this,SLOT(createDashboard()));
    disconnect(ui->pLeftTB,SIGNAL(clicked()),this,SLOT(leftDashboard()));
    disconnect(ui->pRightTB,SIGNAL(clicked()),this,SLOT(rightDashboard()));
    disconnect(ui->pSaveTB,SIGNAL(clicked()),this,SLOT(saveDashboard()));
    disconnect(ui->pSavAsTB,SIGNAL(clicked()),this,SLOT(saveAsDashboard()));
    disconnect(ui->pLoadTB,SIGNAL(clicked()),this,SLOT(loadDashboard()));
    disconnect(ui->pDashboardCB,SIGNAL(currentIndexChanged(int)),this,SLOT(activateDashboard(int)));

    int i=ui->pDashboardCB->currentIndex();
    ui->pLeftTB->setEnabled(_dashboardLst.count()>0 && i>0);
    ui->pRightTB->setEnabled(_dashboardLst.count()>0 && i<_dashboardLst.count()-1);
    pUpdateTimer->stop();
    updateStatusLab();
    pUpdateTimer->start();

    connect(ui->pClearTB,SIGNAL(clicked()),this,SLOT(clearDashboard()));
    connect(ui->pCloseTB,SIGNAL(clicked()),this,SLOT(closeDashboard()));
    connect(ui->pNewTB,SIGNAL(clicked()),this,SLOT(createDashboard()));
    connect(ui->pLeftTB,SIGNAL(clicked()),this,SLOT(leftDashboard()));
    connect(ui->pRightTB,SIGNAL(clicked()),this,SLOT(rightDashboard()));
    connect(ui->pSaveTB,SIGNAL(clicked()),this,SLOT(saveDashboard()));
    connect(ui->pSavAsTB,SIGNAL(clicked()),this,SLOT(saveAsDashboard()));
    connect(ui->pLoadTB,SIGNAL(clicked()),this,SLOT(loadDashboard()));
    connect(ui->pDashboardCB,SIGNAL(currentIndexChanged(int)),this,SLOT(activateDashboard(int)));
}
void xVDashboardToolBar::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type)
    {
    case ST_DASHBOARD_ACTIVATED:
    case ST_DASHBOARD_INFO_MODIFIED:
    case ST_DASHBOARD_CREATED:
        {
            connect(ui->pDashboardCB,SIGNAL(currentIndexChanged(int)),this,SLOT(activateDashboard(int)));
            ui->pDashboardCB->clear();
            QStringList _names;
            int currentId=-1;
            int j=0;
            xVDashBoard *pCurrentDashboard = (xVDashBoard*)pData;
            for (QList <xVDashBoard*>::iterator it=_dashboardLst.begin(); it!=_dashboardLst.end(); ++it) {
                _names << (*it)->_name;
                if ((*it)==pCurrentDashboard) currentId=j;
                ++j;
            }
            ui->pDashboardCB->addItems(_names);
            currentId!=-1 ? ui->pDashboardCB->setCurrentIndex(currentId) : ui->pDashboardCB->setCurrentIndex(_currentDashBoard);
            connect(ui->pDashboardCB,SIGNAL(currentIndexChanged(int)),this,SLOT(activateDashboard(int)));
            updateGui();
        }
        break;
    default:
        // handled
        break;
    }
}

