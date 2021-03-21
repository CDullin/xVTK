#ifndef XVEQUATIONDESIGNDLG_H
#define XVEQUATIONDESIGNDLG_H

#include <QDialog>
#include "xVTypes.h"
#include "xvEvalCondition.h"
#include "xVAbstractBaseObj.h"

namespace Ui {
class xVEquationDesignDlg;
}

class xVEquationDesignDlg : public QDialog
{
    Q_OBJECT

public:
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

    explicit xVEquationDesignDlg(QWidget *parent = nullptr);
    ~xVEquationDesignDlg();

    void addInputParam(const QString& key,xParamMap* p,xVAbstractBaseObj* pVObj=nullptr);
    QString equationStr();
    QString str2equation();
    void setEquation(QString);

    virtual int exec() override;
    virtual bool eventFilter(QObject*,QEvent*) override;

protected slots:
    void moveIn();
    void moveOut();
    void setAsResult();
    void verify();
    void sourceActivated(int);
    void cellDblClicked();

protected:
    QString getCurrentVarFrmTable();

private:
    Ui::xVEquationDesignDlg *ui;
    QMap <QString,DEF_NODE> _whatToDefineMp;
};

#endif // XVEQUATIONDESIGNDLG_H
