#ifndef XVDASHBOARDTOOLBAR_H
#define XVDASHBOARDTOOLBAR_H

#include <QWidget>
#include "xVTypes.h"

namespace Ui {
class xVDashboardToolBar;
}

class xVDashboardToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit xVDashboardToolBar(QWidget *parent = nullptr);
    ~xVDashboardToolBar();

protected slots:
    void closeDashboard();
    void clearDashboard();
    void createDashboard();
    void leftDashboard();
    void rightDashboard();
    void saveDashboard();
    void loadDashboard();
    void saveAsDashboard();
    void updateStatusLab();
    void activateDashboard(int);
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

protected:
    void updateGui();
    QTimer *pUpdateTimer;

private:
    Ui::xVDashboardToolBar *ui;
};

#endif // XVDASHBOARDTOOLBAR_H
