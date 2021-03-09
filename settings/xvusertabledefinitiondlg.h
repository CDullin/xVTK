#ifndef XVUSERTABLEDEFINITIONDLG_H
#define XVUSERTABLEDEFINITIONDLG_H

#include <QDialog>
#include <QToolButton>
#include "xVTypes.h"

namespace Ui {
class xVUserTableDefinitionDlg;
}

struct DEF_NODE
{
public:
    xAbstractBasisObj* pRefObj = nullptr;       // nullptr if global namespace
    xParamMap* pParamMap = nullptr;
    DEF_NODE(){}
    DEF_NODE(xAbstractBasisObj* pO,xParamMap* pP){
        pRefObj=pO;
        pParamMap=pP;
    }
};

class xVUserTableDefinitionDlg : public QDialog
{
    Q_OBJECT

public:
    enum xUTDD_MODE
    {
        xUTDDM_DEFINITION,
        xUTDDM_CONTROL
    };

    explicit xVUserTableDefinitionDlg(QWidget *parent = nullptr);
    ~xVUserTableDefinitionDlg();

    void addInputParam(const QString& key,xParamMap* p,xAbstractBasisObj* pVObj=nullptr);
    void setOutputParam(xParamMap* p,xAbstractBasisObj* pVObj=nullptr);
    void setToDefinitionMode();
    void setToControlMode();

    void accept() override;
    void reject() override;
    int exec() override;

public slots:
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);

protected slots:
    void moveIn();
    void moveOut();
    void addItem();
    void removeItem();
    void sourceActivated(int);

protected:
    bool keyExists();

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

private:
    Ui::xVUserTableDefinitionDlg *ui;
    xParamMap *pCopyParamMp = nullptr;
    xUTDD_MODE _mode = xUTDDM_DEFINITION;
    QMap <QString,DEF_NODE> _whatToDefineMp;
};

#endif // XVUSERTABLEDEFINITIONDLG_H
