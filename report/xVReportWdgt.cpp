#include "xVReportWdgt.h"
#include "xVReport.h"
#include "xVReportWdgtToolBar.h"
#include <QStackedWidget>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>

xVReportWdgt::xVReportWdgt(QWidget* parent):QWidget(parent)
{
    pStackWdgt = new QStackedWidget(this);
    pToolBar = new xVReportWdgtToolBar(this);
    pToolBar->setReportMapRef(&_reportLst);
    QVBoxLayout *pVBox=new QVBoxLayout(this);
    pVBox->addWidget(pToolBar);
    pVBox->addWidget(pStackWdgt);
    pVBox->setMargin(3);

    connect(pToolBar,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL(KSignal(const SIG_TYPE&,void*)));
}

void xVReportWdgt::clear()
{
    for (int i=0;i<pStackWdgt->count();++i)
    {
        QWidget* pWdgt = pStackWdgt->widget(0);
        pStackWdgt->removeWidget(0);
        if (pWdgt) delete pWdgt;
    }

    if (pLab==nullptr)
    {
        pLab = new QLabel(pStackWdgt);
        pLab->setText("no active reports");
        pLab->setGeometry(5,5,pStackWdgt->width()-10,pStackWdgt->height()-10);
        pLab->setAlignment(Qt::AlignCenter);
        pLab->raise();
        pLab->setFocusPolicy(Qt::NoFocus);
    }
    pLab->show();
}

void xVReportWdgt::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (pLab!=nullptr)
        pLab->setGeometry(5,5,width()-10,height()-10);
}

void xVReportWdgt::saveCurrentReport(const QString& fname)
{
    if (pStackWdgt->currentWidget()==nullptr) return;
    QString name=fname;
    if (name.isEmpty()) name=QFileDialog::getSaveFileName(0,"pick name to export report",::_settings["last report file"]._value.value<xFileName>()._fileName,"*.html");
    if (!name.isEmpty()) {
        ::_settings["last report file"]._value=QVariant::fromValue(xFileName(xFileName::FN_OUTPUT_FILE,name));
        ((xVReport*)pStackWdgt->currentWidget())->save(name);
        emit KSignal(ST_REPORT_CREATED);
        emit KSignal(ST_ACTIVATE_REPORT,new QString(((xVReport*)pStackWdgt->currentWidget())->_name));
    }
}

void xVReportWdgt::saveAllReportsToFolder(QString folder)
{
    int id=pStackWdgt->currentIndex();
    for (int i=0;i<_reportLst.count();++i)
    {
        xVReport *pVReport = (xVReport*)pStackWdgt->widget(i);
        pStackWdgt->setCurrentIndex(i);
        QDir d(folder+"/"+pVReport->_name);
        saveCurrentReport(folder+"/"+pVReport->_name+".html");
    }
    pStackWdgt->setCurrentIndex(id);
}

void xVReportWdgt::loadReport(const QString& fname)
{
    QString name=fname;
    if (name.isEmpty()) name=QFileDialog::getOpenFileName(0,"pick name to export report","","*.html");
    if (!name.isEmpty()) {

        QString _currentPath = QDir::currentPath();

        pLab->hide();
        QFileInfo info(name);
        if (!containsReport(info.baseName()))
        {
            QDir::setCurrent(info.absolutePath());
            xVReport *pReport = new xVReport(info.baseName());
            pReport->load(name);

            _reportLst.append(pReport);
            pStackWdgt->addWidget(pReport);
            connect(pReport,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL(KSignal(const SIG_TYPE&,void*)));
            emit KSignal(ST_REPORT_CREATED);
            emit KSignal(ST_ACTIVATE_REPORT,new QString(info.baseName()));
        }
        else emit KSignal(ST_ERROR_MSG,new QString("report is already open"));

        QDir::setCurrent(_currentPath);
    }
}

bool xVReportWdgt::containsReport(const QString& name)
{
    bool found = false;
    for (QList <xVReport*>::iterator it=_reportLst.begin(); it!=_reportLst.end(); ++it) {
        if ((*it)->_name==name)
            found = true;
    }

    return found;
}

int xVReportWdgt::indexOf(const QString& name)
{
    int id = -1;
    for (int i=0;i<_reportLst.count();++i) {
        if (_reportLst[i]->_name==name)
            id=i;
    }
    return id;
}
void xVReportWdgt::activateReport(const QString& name)
{
    if (containsReport(name))
    {
        pStackWdgt->setCurrentWidget(_reportLst[indexOf(name)]);
        emit KSignal(ST_REPORT_ACTIVATED,_reportLst[indexOf(name)]);
    }
}

void xVReportWdgt::createReport(const QString& name)
{
    if (containsReport(name))
    {
        pStackWdgt->setCurrentWidget(_reportLst[indexOf(name)]);
        emit KSignal(ST_REPORT_ACTIVATED,_reportLst[indexOf(name)]);
    }
    else
    {
        xVReport *pReport = new xVReport(name);
        _reportLst.append(pReport);
        pStackWdgt->addWidget(pReport);
        connect(pReport,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL(KSignal(const SIG_TYPE&,void*)));
        emit KSignal(ST_REPORT_CREATED);
        emit KSignal(ST_ACTIVATE_REPORT,new QString(name));
    }
    pLab->hide();
}

void xVReportWdgt::closeReport(xVReport *pReport)
{
    if (pReport && (!pReport->_modified ||
                    QMessageBox::question(0,"Warning","The report contains unsaved modifications. Do you really like to close it without saving?")==QMessageBox::Yes))
    {
        _reportLst.removeOne(pReport);
        delete pReport;
        emit KSignal(ST_REPORT_CREATED);
    }
}

void xVReportWdgt::KSlot(const SIG_TYPE& type,void* pData)
{
    switch (type)
    {
    case ST_CLOSE_REPORT:
        closeReport((xVReport*)pData);
        break;
    case ST_LOAD_REPORT:
        if (pData!=nullptr)
        {
            QString *name=(QString*)pData;
            loadReport(*name);
            delete name;
        }
        else loadReport();
        break;
    case ST_CREATE_REPORT:
    {
        QString *name=(QString*)pData;
        createReport(*name);
        delete name;
    }
        break;
    case ST_ACTIVATE_REPORT:
    {
        QString *name=(QString*)pData;
        activateReport(*name);
        delete name;
    }
        break;
    case ST_SAVE_ALL_REPORTS:
    {
        QString *name=(QString*)pData;
        saveAllReportsToFolder(*name);
        delete name;
    }
        break;
    case ST_SAVE_CURRENT_REPORT:
        if (pData!=nullptr)
        {
            QString *name=(QString*)pData;
            saveCurrentReport(*name);
            delete name;
        }
        else saveCurrentReport();
        break;
    }
    pToolBar->KSlot(type,pData);
}

