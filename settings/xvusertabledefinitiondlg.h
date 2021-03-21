#ifndef XVUSERTABLEDEFINITIONDLG_H
#define XVUSERTABLEDEFINITIONDLG_H

#include <QDialog>
#include <QToolButton>
#include "xVTypes.h"
#include "xVAbstractBaseObj.h"

namespace Ui {
class xVUserTableDefinitionDlg;
}

class xVUserTableDefinitionDlg : public QDialog
{
    Q_OBJECT

public:
    enum xUTDD_MODE
    {
        xUTDDM_DEFINITION,
        xUTDDM_CONTROL
    };

    struct DEF_NODE
    {
    public:
        xVAbstractBaseObj* pRefObj = nullptr;       // nullptr if global namespace
        xParamMap* pParamMap = nullptr;
        DEF_NODE(){}
        DEF_NODE(xVAbstractBaseObj* pO,xParamMap* pP){
            pRefObj=pO;
            pParamMap=pP;
        }
    };

    explicit xVUserTableDefinitionDlg(QWidget *parent = nullptr);
    ~xVUserTableDefinitionDlg();

    void addInputParam(const QString& key,xParamMap* p,xVAbstractBaseObj* pVObj=nullptr);
    void setOutputParam(xParamMap p,xVAbstractBaseObj* pVObj=nullptr);
    void setToDefinitionMode();
    void setToControlMode();
    xParamMap resultingMap();

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
