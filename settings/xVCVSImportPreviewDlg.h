#ifndef XVCVSIMPORTPREVIEWDLG_H
#define XVCVSIMPORTPREVIEWDLG_H

#include <QDialog>

namespace Ui {
class xVCVSImportPreviewDlg;
}

class xVCVSImportPreviewDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVCVSImportPreviewDlg(QWidget *parent = nullptr);
    ~xVCVSImportPreviewDlg();

private:
    Ui::xVCVSImportPreviewDlg *ui;
};

#endif // XVCVSIMPORTPREVIEWDLG_H
