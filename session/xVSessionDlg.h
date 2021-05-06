#ifndef XVSESSIONDLG_H
#define XVSESSIONDLG_H

#include <QDialog>
#include <QTreeWidget>
#include "xVTypes.h"

namespace Ui {
class xVSessionDlg;
}

class xVSessionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVSessionDlg(QWidget *parent = nullptr);
    ~xVSessionDlg();
    virtual void accept() override;
    virtual void reject() override;
    virtual int exec() override;

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

public slots:
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);

protected slots:
    void itemActivated(QTreeWidgetItem *item, int column);
    void loadCurrentItem();
    void deleteSelectedSession();

protected:
    void addDashboards(QTreeWidgetItem *,QString);
    void addReports(QTreeWidgetItem *,QString);
    void updateGui();
    void saveSession();
    void removeCurrentTmpFolder();
    void deleteAllIncompleteSessions();
    void loadSession(QTreeWidgetItem*);
    void loadDashboard(const QString&);
    void loadReport(const QString&);

private:
    Ui::xVSessionDlg *ui;
};

#endif // XVSESSIONDLG_H
