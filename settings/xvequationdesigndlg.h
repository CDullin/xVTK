#ifndef XVEQUATIONDESIGNDLG_H
#define XVEQUATIONDESIGNDLG_H

#include <QDialog>
#include "xVTypes.h"

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
        xAbstractBasisObj* pRefObj = nullptr;       // nullptr if global namespace
        xParamMap* pParamMap = nullptr;
        DEF_NODE(){}
        DEF_NODE(xAbstractBasisObj* pO,xParamMap* pP){
            pRefObj=pO;
            pParamMap=pP;
        }
    };

    explicit xVEquationDesignDlg(QWidget *parent = nullptr);
    ~xVEquationDesignDlg();

    void addInputParam(const QString& key,xParamMap* p,xAbstractBasisObj* pVObj=nullptr);
    QString equation();

protected slots:
    void moveIn();
    void moveOut();
    void setAsResult();
    void verify();
    void sourceActivated(int);

private:
    Ui::xVEquationDesignDlg *ui;
    QMap <QString,DEF_NODE> _whatToDefineMp;
};

#endif // XVEQUATIONDESIGNDLG_H
