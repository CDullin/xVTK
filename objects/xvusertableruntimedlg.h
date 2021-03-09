#ifndef XVUSERTABLERUNTIMEDLG_H
#define XVUSERTABLERUNTIMEDLG_H

#include <QDialog>
#include "xVTypes.h"
#include "xVObjects.h"
#include "xVUserTableImportDlgObj.h"

namespace Ui {
class xVUserTableRuntimeDlg;
}

class xVUserTableRuntimeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVUserTableRuntimeDlg(QWidget *parent = nullptr);
    ~xVUserTableRuntimeDlg();
    xParamMap* map(){return pMapRef;}
    void setMap(xParamMap* m,xVUserTableImportDlgObj*);
    void setTitle(const QString&);
signals:
    void modified();
    void KSignal(const SIG_TYPE& ,void *);

private:
    Ui::xVUserTableRuntimeDlg *ui;
    xParamMap *pMapRef=nullptr;
};

#endif // XVUSERTABLERUNTIMEDLG_H
