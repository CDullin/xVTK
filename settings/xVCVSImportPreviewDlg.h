#ifndef XVCVSIMPORTPREVIEWDLG_H
#define XVCVSIMPORTPREVIEWDLG_H

#include <QDialog>
#include "xVTable.h"

namespace Ui {
class xVCVSImportPreviewDlg;
}

class xVImportCVSObj;

class xVCVSImportPreviewDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVCVSImportPreviewDlg(xVImportCVSObj*,xVTable *,QWidget *parent = nullptr);
    ~xVCVSImportPreviewDlg();

public slots:
    void updateDialog();

protected slots:
    void loadAndDisplayFile();
    void updateAndDisplayTable();

private:
    Ui::xVCVSImportPreviewDlg *ui;
    xVTable *pTabRef = nullptr;
    xVImportCVSObj *pCVSObj = nullptr;
    xParamMap _paramMp;
};

#endif // XVCVSIMPORTPREVIEWDLG_H
