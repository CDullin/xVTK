#ifndef XVLOADDLG_H
#define XVLOADDLG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class xVLoadDlg;
}

class xVLoadDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVLoadDlg(QWidget *parent = nullptr);
    ~xVLoadDlg();
    QString currentDashboard(){return _currentDashboard;}

protected slots:
    void browseFolder(const QString& path="");
    void itemActivated(QListWidgetItem *pItem);
    void loadItem(QListWidgetItem *pItem);
    void updateGui();
    void deleteDashboard();

private:
    Ui::xVLoadDlg *ui;
    QString _currentDashboard;
    QString _lastFolder;
};

#endif // XVLOADDLG_H
