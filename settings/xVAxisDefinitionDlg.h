#ifndef XVAXISDEFINITIONDLG_H
#define XVAXISDEFINITIONDLG_H

#include <QDialog>
#include "xVTypes.h"

namespace Ui {
class xVAxisDefinitionDlg;
}

class xVAxisDefinitionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVAxisDefinitionDlg(QWidget *parent = nullptr);
    ~xVAxisDefinitionDlg();
    void setParameterRef(xParamMap* pRef);
    xParamMap *result(){return pParamMpRef;}
    virtual void accept() override;
    virtual void reject() override;

signals:
    void KSignal(const SIG_TYPE&,void *);

public slots:
    void KSlot(const SIG_TYPE&,void *);

private:
    Ui::xVAxisDefinitionDlg *ui;
    xParamMap *pCopyParamMp = nullptr;
    xParamMap *pParamMpRef = nullptr;
};

#endif // XVAXISDEFINITIONDLG_H
