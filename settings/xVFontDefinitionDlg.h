#ifndef XVFONTDEFINITIONDLG_H
#define XVFONTDEFINITIONDLG_H

#include <QDialog>
#include "xVTypes.h"

namespace Ui {
class xVFontDefinitionDlg;
}

class xVFontDefinitionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVFontDefinitionDlg(QWidget *parent = nullptr);
    ~xVFontDefinitionDlg();
    void setParameterRef(xParamMap* pRef);
    xParamMap *result(){return pParamMpRef;}
    virtual void accept() override;
    virtual void reject() override;
    virtual int exec() override;

signals:
    void KSignal(const SIG_TYPE&,void *);

public slots:
    void KSlot(const SIG_TYPE&,void *);

protected slots:
    void displayFont();

private:
    Ui::xVFontDefinitionDlg *ui;
    xParamMap *pCopyParamMp = nullptr;
    xParamMap *pParamMpRef = nullptr;
};

#endif // XVFONTDEFINITIONDLG_H
