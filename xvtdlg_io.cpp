#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVTypes.h"
#include "xvsavedlg.h"
#include "xvloaddlg.h"
#include <QDir>
#include <QFile>
#include <QDataStream>
#include "xVObjectTypes.h"
#include <QMessageBox>

void xVTKDlg::save()
{
    if (_objLst.count()>0)
    {
        QRectF _sceneRect=ui->pDashBoardGV->scene()->itemsBoundingRect();
        QPixmap pix(_sceneRect.width()+40,_sceneRect.height()+40);
        QRectF _copy(_sceneRect.x()-20,_sceneRect.y()-20,_sceneRect.width()+40,_sceneRect.height()+40);
        QPainter pain(&pix);
        ui->pDashBoardGV->scene()->render(&pain,QRectF(0,0,_sceneRect.width()+40,_sceneRect.height()+40),_copy);
        xVSaveDlg dlg;
        dlg.setPixmap(pix);
        if (dlg.exec()==QDialog::Accepted)
        {
            QString _currentPath = QDir::currentPath();
            QDir d;
            d.mkpath(dlg.path());
            QDir::setCurrent(dlg.path());

            QFile f(dlg.path()+"/dashboard.dat");
            if (f.open(QFile::WriteOnly))
            {
                bool _explicit = dlg.saveExplicit();

                QDataStream ds(&f);
                ds << pix;
                ds << dlg.header();
                ds << _explicit;
                ds << QString("header_end");
                ds << (quint16)_objLst.count();
                QList<xVObj_Basics*> baseObjLst;
                QList<xConnectorObj*> conLst;
                for (QList<xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
                {
                    xVObj_Basics *pVObj=dynamic_cast<xVObj_Basics*>(*it);
                    xConnectorObj *pConObj=dynamic_cast<xConnectorObj*>(*it);
                    if (pVObj) baseObjLst.append(pVObj);
                    if (pConObj) conLst.append(pConObj);
                }

                // write objlst
                // we just export all the objects and leave the connected object lists empty
                ds << (quint16) baseObjLst.count();
                for (QList<xVObj_Basics*>::iterator it=baseObjLst.begin();it!=baseObjLst.end();++it)
                {
                    ds << (quint16)(*it)->type();
                    // id, parameter, (referenced data), position in the scene
                    (*it)->save(ds,_explicit);
                    // save position in scene
                    ds << (*it)->item()->pos();
                }

                // later we read the connectors after the objects and use the id to connect them again
                ds << (quint16) conLst.count();
                for (QList<xConnectorObj*>::iterator it=conLst.begin();it!=conLst.end();++it)
                {
                    // id, parameter, ids of left and right, position in the scene
                    (*it)->save(ds,_explicit);
                }

                f.close();

                QMessageBox::information(0,"information","export finished successfully");
            }
            else
                emit KSignal(ST_ERROR_MSG,new QString("could not create export file"));

            QDir::setCurrent(_currentPath);
        }
        else
            emit KSignal(ST_WARN_MSG,new QString("export aborted by user"));
    }
    else
    {
        emit KSignal(ST_WARN_MSG,new QString("the dash board appears to be empty. export aborted"));
    }
}

xAbstractBasisObj* xVTKDlg::getObjById(const QString& id)
{
    xAbstractBasisObj* pResObj=nullptr;
    for (int i=0;i<_objLst.count();++i)
        if (_objLst[i]->id()==id)
            pResObj=_objLst[i];

    return pResObj;
}

xConnector* xVTKDlg::getConnectorById(const QString & id)
{
    xConnector* pResCon=nullptr;
    for (int i=0;i<_objLst.count();++i)
    {
        xVObj_Basics *pCurrentVObj=dynamic_cast<xVObj_Basics*>(_objLst[i]);
        if (pCurrentVObj)
            for (int j=0;j<pCurrentVObj->connectorLst()->count();++j)
                if (pCurrentVObj->connectorLst()->at(j)->id()==id)
                    pResCon = pCurrentVObj->connectorLst()->at(j);
    }
    return pResCon;
}

void xVTKDlg::load()
{
    if (_objLst.count()>0)
    {
        if (QMessageBox::question(0,"Warning","Laoding will remove all current data. Do you like to continue")==QMessageBox::No)
            return;
    }
    clear();

    xVLoadDlg dlg;
    if (dlg.exec()==QDialog::Accepted)
    {
        bool _abort=false;
        QFile f(dlg.currentDashboard());
        if (f.open(QFile::ReadOnly))
        {
            QDataStream d(&f);
            QPixmap pix; d >> pix;
            QStringList header; d >> header;
            bool _explicit; d >> _explicit;
            QString hStr;d >> hStr;
            quint16 _count; d >> _count;
            ui->pProg->setRange(0,_count-1);
            quint16 _baseObjCount; d >> _baseObjCount;
            // load base objects
            for (int i=0;i<_baseObjCount && !_abort;++i) {
                quint16 _type;d >> _type;
                xVObj_Basics* pObj=nullptr;
                switch ((xVO_TYPE)_type)
                {
                case xVOT_VOLUME:           pObj=new xVVolObj(d);               break;
                case xVOT_MESH:             pObj=new xVPolyObj(d);              break;
                case xVOT_IMAGE:            break;
                case xVOT_IMAGE_STACK:      break;
                case xVOT_PLANE:            break;
                case xVOT_2D_VIEW:          pObj=new xV2DVisObj(d); connect(pObj,SIGNAL(addVisWdgt(QWidget*)),this,SLOT(addVisWidget(QWidget*)));   break;
                case xVOT_3D_VIEW:          pObj=new xV3DVisObj(d); connect(pObj,SIGNAL(addVisWdgt(QWidget*)),this,SLOT(addVisWidget(QWidget*)));   break;
                case xVOT_VOLUME_VIS_PROP:  pObj=new xVVolumeVisPropObj(d);     break;
                case xVOT_MESH_VIS_PROP:    pObj=new xVMeshVisPropObj(d);       break;
                case xVOT_USER_TABLE_DLG:   pObj=new xVUserTableImportDlgObj(d);break;
                case xVOT_VAR_DEFINITION:   pObj=new xVVarDefinitionObj(d);     break;
                case xVOT_TRAFFIC_LIGHT:    pObj=new xVTrafficLightObj(d);      break;
                case xVOT_MATH:             pObj=new xVMathObj(d);              break;
                default:
                    emit KSignal(ST_ERROR_MSG,new QString("invalid object type found. import aborted"));
                    _abort=true;
                    continue;
                }

                QPointF pnt; d >> pnt;

                if (pObj)
                {
                    ui->pDashBoardGV->scene()->addItem(pObj->item());
                    connect(pObj,SIGNAL(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)));
                    emit KSignal(ST_ADD_OBJECT,pObj);
                    pObj->item()->setPos(pnt);
                }
            }

            // convert all ids of the connectors to real references
            for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
            {
                xVObj_Basics* pVObj=dynamic_cast<xVObj_Basics*>(*it);
                if (pVObj)
                    for (QList<xConnector*>::iterator it2=pVObj->connectorLst()->begin();it2!=pVObj->connectorLst()->end();++it2)
                    {
                        if ((*it2)->objIDs()->count()>0)
                        {
                            QString vObjId = (*it2)->objIDs()->takeFirst();
                            xVObj_Basics* pCurrentVObj = dynamic_cast<xVObj_Basics*>(getObjById(vObjId));
                            if (pCurrentVObj)
                                (*it2)->addConObject(pCurrentVObj);
                        }
                    }
            }

            quint16 _conLstCount; d >> _conLstCount;
            for (int i=0;i<_conLstCount;++i)
            {
                xConnectorObj *pConObj=new xConnectorObj(d);
                QString inId,outId;
                d >> inId;
                d >> outId;
                pConObj->setInputObj(getConnectorById(inId));
                pConObj->setOutputObj(getConnectorById(outId));
                pConObj->generateShape();
                ui->pDashBoardGV->scene()->addItem(pConObj->item());
                emit KSignal(ST_ADD_OBJECT,pConObj);    
            }


            f.close();
        }
        else
            emit KSignal(ST_WARN_MSG,new QString("couldn't open dashboard file"));
    }
    else
        emit KSignal(ST_WARN_MSG,new QString("export aborted by user"));
}
