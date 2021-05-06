#ifndef XVREPORTWDGTTOOLBAR_H
#define XVREPORTWDGTTOOLBAR_H

#include <QWidget>
#include "xVReport.h"
#include "xVTypes.h"

namespace Ui {
class xVReportWdgtToolBar;
}

class xVReportWdgtToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit xVReportWdgtToolBar(QWidget *parent = nullptr);
    ~xVReportWdgtToolBar();
    void setReportMapRef(QList <xVReport*>*);
protected:
    void updateToolbar();
    void updateLimits();
public slots:
    virtual void KSlot(const SIG_TYPE&,void* data=nullptr);
    void createReport();
    void leftReport();
    void rightReport();
    void saveReport();
    void saveAsReport();
    void clearReport();
    void closeReport();
    void loadReport();
    void activateReport(int);

protected slots:
    void updateTimerTimeOut();

signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
private:
    Ui::xVReportWdgtToolBar *ui;    
    QList <xVReport*> *pRef = nullptr;
};

#endif // XVREPORTWDGTTOOLBAR_H
