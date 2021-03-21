#include "xvequationdesigndlg.h"
#include "ui_xvequationdesigndlg.h"
#include "xvproptable.h"
#include "xVObjects.h"

#include <QKeyEvent>

xVEquationDesignDlg::xVEquationDesignDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVEquationDesignDlg)
{
    ui->setupUi(this);
    ui->pEquationLEdit->installEventFilter(this);
    connect(ui->pSrcPropTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDblClicked()));
    connect(ui->pMoveInTB,SIGNAL(clicked()),this,SLOT(moveIn()));
    connect(ui->pMoveOutTB,SIGNAL(clicked()),this,SLOT(moveOut()));
    connect(ui->pSetAsResultTB,SIGNAL(clicked()),this,SLOT(setAsResult()));
    connect(ui->pControlledObjCB,SIGNAL(currentIndexChanged(int)),this,SLOT(sourceActivated(int)));
}

void xVEquationDesignDlg::cellDblClicked()
{
    if (ui->pEquationLEdit->text().contains(":=")) ui->pMoveInTB->animateClick();
    else ui->pSetAsResultTB->animateClick();
}

bool xVEquationDesignDlg::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==ui->pEquationLEdit && event->type()==QEvent::KeyPress)
    {
        QKeyEvent *ke=(QKeyEvent*)event;
        switch (ke->key())
        {
        case Qt::Key_Delete:
        {
            QString s= ui->pEquationLEdit->text();
            ui->pEquationLEdit->setCursorPosition(ui->pEquationLEdit->cursorPosition()+1);
            moveOut();
            if (s!=ui->pEquationLEdit->text())
                return true;
            else
                ui->pEquationLEdit->setCursorPosition(ui->pEquationLEdit->cursorPosition()-1);
        }
            break;
        case Qt::Key_Backspace:
        {
            QString s= ui->pEquationLEdit->text();
            ui->pEquationLEdit->setCursorPosition(ui->pEquationLEdit->cursorPosition()-1);
            moveOut();
            if (s!=ui->pEquationLEdit->text())
                return true;
            else
                ui->pEquationLEdit->setCursorPosition(ui->pEquationLEdit->cursorPosition()+1);
        }
            break;
        }
    }
    return QDialog::eventFilter(watched,event);
}

int xVEquationDesignDlg::exec()
{
    if (_whatToDefineMp.count()>0) sourceActivated(0);
    ui->pSrcPropTable->setOnlyRealDataTypesVisible(true);
    return QDialog::exec();
}

QString xVEquationDesignDlg::equationStr()
{
    return ui->pEquationLEdit->text();
}

void xVEquationDesignDlg::moveIn()
{
    ui->pEquationLEdit->insert(getCurrentVarFrmTable());
    QString s=ui->pEquationLEdit->text();
    for (int j=1;j<s.length()-1;++j) {
        if (s[j]=="}" && s[j+1]!=" ") s.insert(j+1," ");
        if (s[j]=="{" && s[j-1]!=" ") s.insert(j," ");
    }
    ui->pEquationLEdit->setText(s);
}

void xVEquationDesignDlg::moveOut()
{
    QString s=ui->pEquationLEdit->text();
    // check if we are in a variable
    int pos = ui->pEquationLEdit->cursorPosition();
    int leftPosOpening = s.lastIndexOf("{",pos);
    int leftPosClosing = s.lastIndexOf("}",pos);
    int rightPosOpening = s.indexOf("{",pos);
    int rightPosClosing = s.indexOf("}",pos);

    if (leftPosOpening>-1 && leftPosOpening<=pos && (leftPosOpening>leftPosClosing || leftPosClosing==pos) && rightPosClosing>-1)
    {
        s=s.replace(leftPosOpening,rightPosClosing-leftPosOpening+1,"");
        s=s.simplified();
        ui->pEquationLEdit->setText(s);
        ui->pEquationLEdit->setCursorPosition(leftPosOpening);
    }

}

QString xVEquationDesignDlg::getCurrentVarFrmTable()
{
    QString res="";

    QList <QTableWidgetItem*>_selectedItemsLst=ui->pSrcPropTable->selectedItems();
    if (_selectedItemsLst.count()>0)
    {
        int r=_selectedItemsLst[0]->row();
        QString key = ui->pSrcPropTable->item(r,0)->text();
        QString reference = ui->pControlledObjCB->currentText();

        res = "{"+key+"|"+reference+"}";
    }

    return res;
}

void xVEquationDesignDlg::setAsResult()
{
    QString s=ui->pEquationLEdit->text();
    s = getCurrentVarFrmTable()+" := "+s.section(" := ",1,1);
    ui->pEquationLEdit->setText(s);
}

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

void xVEquationDesignDlg::setEquation(QString e)
{
    if (e=="0") e="";
    for (int i=0;i<e.length();++i)
    {
        if (e[i]=="{")  // start var blk
        {
            int endPos = e.indexOf("}",i);
            QString vblk = e.mid(i+1,endPos-1);
            QString var = vblk.section("|",0,0);
            quint64 objPtr = vblk.section("|",1,1).toLongLong();
            QString objName = "global";
            if (objPtr!=0)
            {
                xVObj_Basics *pVObj=(xVObj_Basics*)objPtr;
                objName = (*pVObj->paramMap())["name"]._value.toString();
            }
            e.replace(i,endPos-i+1,"{"+var+"|"+objName+"}");
        }
    }
    ui->pEquationLEdit->setText(e);
}

QString xVEquationDesignDlg::str2equation()
{
    QString e=ui->pEquationLEdit->text();
    for (int i=0;i<e.length();++i)
    {
        if (e[i]=="{")  // start var blk
        {
            int endPos = e.indexOf("}",i);
            QString vblk = e.mid(i+1,endPos-1);
            QString var = vblk.section("|",0,0);
            QString objName = vblk.section("|",1,1);
            QString objPtr=0;
            if (objName!="global")
            {
                xVAbstractBaseObj* pBasePtr=objId2objPtr(objName2objId(objName));
                xVObj_Basics *pVObj=dynamic_cast<xVObj_Basics*>(pBasePtr);
                objPtr = QString("%1").arg((quint64)pVObj);
            }
            e.replace(i,endPos-i+1,"{"+var+"|"+objPtr+"}");
        }
    }
    return e;
}

void xVEquationDesignDlg::addInputParam(const QString& key,xParamMap* p,xVAbstractBaseObj* pVObj)
{
    if (_whatToDefineMp.contains(key)) return;
    ui->pControlledObjCB->addItem(key);
    _whatToDefineMp[key]=DEF_NODE(pVObj,p);
}
