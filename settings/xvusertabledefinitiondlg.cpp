#include "xvusertabledefinitiondlg.h"
#include "ui_xvusertabledefinitiondlg.h"

#include <QDir>

#include <stdlib.h>

using namespace std;

xVUserTableDefinitionDlg::xVUserTableDefinitionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVUserTableDefinitionDlg)
{
    ui->setupUi(this);
    connect(ui->pMoveInTB,SIGNAL(clicked()),this,SLOT(moveIn()));
    connect(ui->pMoveOutTB,SIGNAL(clicked()),this,SLOT(moveOut()));
    connect(ui->pAddTB,SIGNAL(clicked()),this,SLOT(addItem()));
    connect(ui->pRemoveTB,SIGNAL(clicked()),this,SLOT(removeItem()));
    connect(ui->pControlledObjCB,SIGNAL(currentIndexChanged(int)),this,SLOT(sourceActivated(int)));
    connect(ui->pPortCB,SIGNAL(currentIndexChanged(int)),this,SLOT(sourceActivated(int)));
    setToDefinitionMode();

    QStringList dataTypes;
    dataTypes << "Bool" << "Integer" << "Double" << "String" << "File name" << "Path name";
    ui->pTypeCB->insertItems(0,dataTypes);
}

xVUserTableDefinitionDlg::~xVUserTableDefinitionDlg()
{
    delete ui;
}

int xVUserTableDefinitionDlg::exec()
{
    qApp->processEvents();
    sourceActivated(ui->pControlledObjCB->currentIndex());
    return QDialog::exec();
}

void xVUserTableDefinitionDlg::setToDefinitionMode()
{
    _mode = xUTDDM_DEFINITION;
    ui->pSrcPropTable->hide();
    ui->pControlLab->hide();
    ui->pMoveInTB->hide();
    ui->pMoveOutTB->hide();
    ui->pDefLab->setText("parameter definition");
    ui->pControlledObjCB->hide();
    ui->pPortCB->hide();

    ui->pVarNameLEdit->show();
    ui->pTypeCB->show();
    ui->pAddTB->show();
    ui->pRemoveTB->show();
    setFixedSize(500,270);
}

void xVUserTableDefinitionDlg::setToControlMode()
{
    _mode = xUTDDM_CONTROL;
    ui->pSrcPropTable->show();
    ui->pControlLab->show();
    ui->pMoveInTB->show();
    ui->pMoveOutTB->show();
    ui->pDefLab->setText("defined input table");
    ui->pControlledObjCB->show();
    ui->pPortCB->show();

    ui->pVarNameLEdit->hide();
    ui->pTypeCB->hide();
    ui->pAddTB->hide();
    ui->pRemoveTB->hide();
    setFixedSize(800,270);

}

void xVUserTableDefinitionDlg::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type)
    {
    case ST_GLOBAL_NAMESPACE_MODIFIED:
        if (_mode == xUTDDM_DEFINITION)
            ui->pTgtPropTable->updateTable(&::_globalNameSpace);
        break;
    default:
        // unhandeled
        break;
    }
}


void xVUserTableDefinitionDlg::addItem()
{
    if (ui->pTgtPropTable->paramMap())
    {
           QString key = ui->pVarNameLEdit->text();
           if (key.isEmpty() || ::_globalNameSpace.contains(key))
           {
               long id=1;
               if (key.isEmpty()) key="VAR";
               QString s=key+QString("%1").arg(id);
               while (::_globalNameSpace.contains(s))
               {
                  ++id;
                  s=key+QString("%1").arg(id);
               }
               key=s;
           }

           quint32 _id=1;
           for (QMap<QString,xPROP_TYPE>::iterator it=::_globalNameSpace.begin();it!=::_globalNameSpace.end();++it)
               _id = max(_id,it->_id);

           switch (ui->pTypeCB->currentIndex())
           {
           case 0 : ::_globalNameSpace[key]._value = true;break;
           case 1 : ::_globalNameSpace[key]._value = QVariant::fromValue(xLimitedInt(0,-32000,32000));break;
           case 2 : ::_globalNameSpace[key]._value = QVariant::fromValue(xLimitedDouble(0,-1E6,1E6,10));break;
           case 3 : ::_globalNameSpace[key]._value = "undefined";break;
           case 4 : ::_globalNameSpace[key]._value = QVariant::fromValue(QFileInfo("unknown"));break;
           case 5 : ::_globalNameSpace[key]._value = QVariant::fromValue(QFileInfo(QDir::currentPath()));break;
           }

           ::_globalNameSpace[key]._id = _id+1;

           emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);
    }
    else emit KSignal(ST_ERROR_MSG,new QString("no active global param map found"));
}

void xVUserTableDefinitionDlg::removeItem()
{
    if (ui->pTgtPropTable->paramMap() && ui->pTgtPropTable->selectedItems().count()>0)
    {
        int r=ui->pTgtPropTable->selectedItems().at(0)->row();
        _globalNameSpace.remove(ui->pTgtPropTable->item(r,0)->text());
        emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);
    }
    else emit KSignal(ST_MSG,new QString("no selected item to remove found"));
}

void xVUserTableDefinitionDlg::setOutputConnectionsEnabled(bool b)
{
    if (b) ui->pPortCB->setEnabled(true);
    else
    {
        ui->pPortCB->setEnabled(false);
        ui->pPortCB->setCurrentIndex(0);
    }
}

void xVUserTableDefinitionDlg::addInputParam(const QString& key,xParamMap* p,xParamMap* pO,xVAbstractBaseObj* pVObj)
{
    if (_whatToDefineMp.contains(key)) return;
    ui->pControlledObjCB->addItem(key);
    _whatToDefineMp[key];
    _whatToDefineMp[key].pRefObj=pVObj;
    _whatToDefineMp[key].pParamMap=p;
    _whatToDefineMp[key].pOutputParamMap=pO;
}

void xVUserTableDefinitionDlg::sourceActivated(int i)
{
    if (_whatToDefineMp.contains(ui->pControlledObjCB->currentText()))
    {
        qApp->processEvents();
        ui->pSrcPropTable->hide();
        if (ui->pPortCB->currentText()=="input")
            ui->pSrcPropTable->updateTable(_whatToDefineMp[ui->pControlledObjCB->currentText()].pParamMap,
                    _whatToDefineMp[ui->pControlledObjCB->currentText()].pRefObj);
        else
            ui->pSrcPropTable->updateTable(_whatToDefineMp[ui->pControlledObjCB->currentText()].pOutputParamMap,
                    _whatToDefineMp[ui->pControlledObjCB->currentText()].pRefObj);
        ui->pSrcPropTable->show();
    }
}

void xVUserTableDefinitionDlg::setOutputParam(xParamMap p,xVAbstractBaseObj* pVObj)
{
    pCopyParamMp=new xParamMap(p);
    ui->pTgtPropTable->updateTable(new xParamMap(p),pVObj);
}

void xVUserTableDefinitionDlg::moveIn()
{
    QList <QTableWidgetSelectionRange> lst=ui->pSrcPropTable->selectedRanges();
    if (lst.count()>0)
    {
        QString key = ui->pSrcPropTable->item(lst.at(0).topRow(),0)->text();
        quint32 _id=1;
        for (QMap<QString,xPROP_TYPE>::iterator it=ui->pTgtPropTable->paramMap()->begin();it!=ui->pTgtPropTable->paramMap()->end();++it)
            _id = max(_id,it->_id);
        if (!ui->pTgtPropTable->keyExists(key) &&
                (*ui->pSrcPropTable->paramMap())[key]._value.userType()!=QMetaType::type(("xParamMap")))    // we dont add maps
        {
            (*ui->pTgtPropTable->paramMap())[key]=(*ui->pSrcPropTable->paramMap())[key];
            (*ui->pTgtPropTable->paramMap())[key].pRefObj = _whatToDefineMp[ui->pControlledObjCB->currentText()].pRefObj;
            (*ui->pTgtPropTable->paramMap())[key]._id = _id+1;
            ui->pTgtPropTable->updateTable();
        }
    }
}

void xVUserTableDefinitionDlg::moveOut()
{
    QList <QTableWidgetSelectionRange> lst=ui->pTgtPropTable->selectedRanges();
    if (lst.count()>0)
    {
        QString key = ui->pTgtPropTable->item(lst.at(0).topRow(),0)->text();
        (*ui->pTgtPropTable->paramMap()).remove(key);
        ui->pTgtPropTable->updateTable();
    }
}

xParamMap xVUserTableDefinitionDlg::resultingMap()
{
    return (*ui->pTgtPropTable->paramMap());
}

void xVUserTableDefinitionDlg::accept()
{
    if (_mode==xUTDDM_DEFINITION) emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);
    QDialog::accept();
}

void xVUserTableDefinitionDlg::reject()
{
    (*ui->pTgtPropTable->paramMap())=*pCopyParamMp;
    if (_mode==xUTDDM_DEFINITION) emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);
    QDialog::reject();
}
