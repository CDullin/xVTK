#ifndef XVREPORTWDGT_H
#define XVREPORTWDGT_H

#include "xVTypes.h"
#include "xVReport.h"
#include <QToolBox>
#include <QLabel>
#include <QStackedWidget>
#include "xVReportWdgtToolBar.h"

class xVReportWdgt:public QWidget
{
    Q_OBJECT
public:
    xVReportWdgt(QWidget* parent=nullptr);
    void saveCurrentReport(const QString& fname="");
    void loadReport(const QString& fname="");
    void activateReport(const QString&);
    void createReport(const QString&);
    void clear();
    void closeReport(xVReport*);
public slots:
    virtual void KSlot(const SIG_TYPE&,void* data=nullptr);
signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
protected:
    void saveAllReportsToFolder(QString folder);
    bool containsReport(const QString&);
    int indexOf(const QString& name);
    virtual void resizeEvent(QResizeEvent*) override;
    QList <xVReport*> _reportLst;
    QLabel *pLab = nullptr;
    QStackedWidget *pStackWdgt = nullptr;
    xVReportWdgtToolBar *pToolBar = nullptr;
};

#endif // XVREPORTWDGT_H
