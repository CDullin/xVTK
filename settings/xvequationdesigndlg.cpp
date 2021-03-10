#include "xvequationdesigndlg.h"
#include "ui_xvequationdesigndlg.h"

xVEquationDesignDlg::xVEquationDesignDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVEquationDesignDlg)
{
    ui->setupUi(this);

    connect(ui->pMoveInTB,SIGNAL(clicked()),this,SLOT(moveIn()));
    connect(ui->pMoveOutTB,SIGNAL(clicked()),this,SLOT(moveOut()));
    connect(ui->pSetAsResultTB,SIGNAL(clicked()),this,SLOT(setAsResult()));
    connect(ui->pControlledObjCB,SIGNAL(currentIndexChanged(int)),this,SLOT(sourceActivated(int)));
}

QString xVEquationDesignDlg::equation()
{
    return ui->pEquationLEdit->text();
}

void xVEquationDesignDlg::moveIn()
{}

void xVEquationDesignDlg::moveOut()
{}

void xVEquationDesignDlg::setAsResult()
{}

void xVEquationDesignDlg::verify()
{}

xVEquationDesignDlg::~xVEquationDesignDlg()
{
    delete ui;
}

void xVEquationDesignDlg::sourceActivated(int i)
{
    if (_whatToDefineMp.contains(ui->pControlledObjCB->currentText()))
    {
        ui->pSrcPropTable->updateTable(_whatToDefineMp[ui->pControlledObjCB->currentText()].pParamMap,
                _whatToDefineMp[ui->pControlledObjCB->currentText()].pRefObj);
    }
}

void xVEquationDesignDlg::addInputParam(const QString& key,xParamMap* p,xAbstractBasisObj* pVObj)
{
    if (_whatToDefineMp.contains(key)) return;
    ui->pControlledObjCB->addItem(key);
    _whatToDefineMp[key]=DEF_NODE(pVObj,p);
}
