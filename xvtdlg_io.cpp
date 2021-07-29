#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVTypes.h"
#include "xvsavedlg.h"
#include "xvloaddlg.h"
#include <QDir>
#include <QFile>
#include <QDataStream>
#include "xVObjectTypes.h"
#include "xVDashboardView.h"
#include <QMessageBox>

void xVTKDlg::autoSave(xVDashBoard *pDashBoard)
{
    if (pDashBoard==nullptr) return;

    QDateTime dt = QDateTime::currentDateTime();
    QString fname = ::_settings["temp folder"]._value.value<xFileName>()._fileName+"/"+::_session+"/dashboards"+"/"+pDashBoard->_name;
    QDir d(fname);
    if (d.mkpath(fname))
    {
        // remove all old tmp files
        QFileInfoList lst=d.entryInfoList(QDir::Files);
        while (lst.count())
        {
            QFileInfo info = lst.takeLast();
            QFile::remove(info.absoluteFilePath());
        }
        fname+="/dashboard"+QString("_")+dt.toString("yyMMddhhmmsszzz")+".dat";
        QFileInfo info(fname);
        pDashBoard->_lastAutoSaved = dt;

        if (pDashBoard->_objLst.count()>0)
        {
            QRectF _sceneRect=pDashBoard->pDashBoardGV->scene()->itemsBoundingRect();
            QPixmap pix(_sceneRect.width()+40,_sceneRect.height()+40);
            QRectF _copy(_sceneRect.x()-20,_sceneRect.y()-20,_sceneRect.width()+40,_sceneRect.height()+40);
            QPainter pain(&pix);
            _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->render(&pain,QRectF(0,0,_sceneRect.width()+40,_sceneRect.height()+40),_copy);

            if (!fname.isEmpty())
            {
                QString _currentPath = QDir::currentPath();
                QDir d;
                d.mkpath(info.absolutePath());
                QDir::setCurrent(info.absolutePath());

                QFile f(fname);
                if (f.open(QFile::WriteOnly))
                {
                    QDataStream ds(&f);
                    ds << pix;
                    ds << _dashboardLst[_currentDashBoard]->_header;
                    ds << false;
                    ds << QString("header_end");
                    ds << (quint16)_dashboardLst[_currentDashBoard]->_objLst.count();
                    QList<xVObj_Basics*> baseObjLst;
                    QList<xConnectorObj*> conLst;
                    for (QList<xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
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
                        quint64 position=ds.device()->pos();

                        ds << (quint32)(*it)->type();
                        // id, parameter, (referenced data), position in the scene
                        (*it)->save(ds,false);
                        // save position in scene
                        ds << (*it)->item()->pos();
                    }

                    // later we read the connectors after the objects and use the id to connect them again
                    ds << (quint16) conLst.count();
                    for (QList<xConnectorObj*>::iterator it=conLst.begin();it!=conLst.end();++it)
                    {
                        // id, parameter, ids of left and right, position in the scene
                        (*it)->save(ds,false);
                    }

                    f.close();
                    emit KSignal(ST_MSG,new QString(QString("dashboard %1 successfully autosaved").arg(pDashBoard->_name)));
                }
                else
                    emit KSignal(ST_ERROR_MSG,new QString("could not create export file"));

                QDir::setCurrent(_currentPath);
            }
        }
    }
    else
        emit KSignal(ST_ERROR_MSG,new QString("Couldn't create tmp dir"));

}

void xVTKDlg::save(bool _forceDialog)
{
    if (_dashboardLst[_currentDashBoard]->_objLst.count()>0)
    {
        QRectF _sceneRect=_dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->itemsBoundingRect();
        QPixmap pix(_sceneRect.width()+40,_sceneRect.height()+40);
        QRectF _copy(_sceneRect.x()-20,_sceneRect.y()-20,_sceneRect.width()+40,_sceneRect.height()+40);
        QPainter pain(&pix);
        _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->render(&pain,QRectF(0,0,_sceneRect.width()+40,_sceneRect.height()+40),_copy);

        QString fname;
        if (_dashboardLst[_currentDashBoard]->_boundToRealFileName && !_forceDialog) fname = _dashboardLst[_currentDashBoard]->_fileName;
        else
        {
            xVSaveDlg dlg;
            dlg.setPixmap(pix);
            if (dlg.exec()==QDialog::Accepted)
            {
                fname=dlg.path();
                _dashboardLst[_currentDashBoard]->_explicit = dlg.saveExplicit();
                _dashboardLst[_currentDashBoard]->_header = dlg.header();
            }
        }

        if (!fname.isEmpty())
        {
            QString _currentPath = QDir::currentPath();
            QDir d;
            d.mkpath(fname);
            QDir::setCurrent(fname);
            _dashboardLst[_currentDashBoard]->_boundToRealFileName = true;
            _dashboardLst[_currentDashBoard]->_fileName=fname;
            _dashboardLst[_currentDashBoard]->_name = fname.section("/",-1,-1);

            QFile f(fname+"/dashboard.dat");
            if (f.open(QFile::WriteOnly))
            {
                QDataStream ds(&f);
                ds << pix;
                ds << _dashboardLst[_currentDashBoard]->_header;
                ds << _dashboardLst[_currentDashBoard]->_explicit;
                ds << QString("header_end");
                ds << (quint16)_dashboardLst[_currentDashBoard]->_objLst.count();
                QList<xVObj_Basics*> baseObjLst;
                QList<xConnectorObj*> conLst;
                for (QList<xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
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
                    quint64 position=ds.device()->pos();
                    ds << (quint32)(*it)->type();
                    emit KSignal(ST_MSG,new QString(QString("saving %1 %2").arg(position,6,16,QChar('0')).arg((*it)->type())));

                    // id, parameter, (referenced data), position in the scene
                    (*it)->save(ds,_dashboardLst[_currentDashBoard]->_explicit);
                    // save position in scene
                    ds << (*it)->item()->pos();
                }

                // later we read the connectors after the objects and use the id to connect them again
                ds << (quint16) conLst.count();
                for (QList<xConnectorObj*>::iterator it=conLst.begin();it!=conLst.end();++it)
                {
                    // id, parameter, ids of left and right, position in the scene
                    (*it)->save(ds,_dashboardLst[_currentDashBoard]->_explicit);
                }

                f.close();

                QMessageBox::information(0,"information","export finished successfully");
                _dashboardLst[_currentDashBoard]->_lastSaved=QDateTime::currentDateTime();
                emit KSignal(ST_DASHBOARD_INFO_MODIFIED,_dashboardLst[_currentDashBoard]);
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

xVAbstractBaseObj* xVTKDlg::getObjById(const QString& id)
{
    xVAbstractBaseObj* pResObj=nullptr;
    for (int i=0;i<_dashboardLst[_currentDashBoard]->_objLst.count();++i)
        if (_dashboardLst[_currentDashBoard]->_objLst[i]->id()==id)
            pResObj=_dashboardLst[_currentDashBoard]->_objLst[i];

    return pResObj;
}

xConnector* xVTKDlg::getConnectorById(const QString & id)
{
    xConnector* pResCon=nullptr;
    for (int i=0;i<_dashboardLst[_currentDashBoard]->_objLst.count();++i)
    {
        xVObj_Basics *pCurrentVObj=dynamic_cast<xVObj_Basics*>(_dashboardLst[_currentDashBoard]->_objLst[i]);
        if (pCurrentVObj)
            for (int j=0;j<pCurrentVObj->connectorLst()->count();++j)
                if (pCurrentVObj->connectorLst()->at(j)->id()==id)
                    pResCon = pCurrentVObj->connectorLst()->at(j);
    }
    return pResCon;
}

void xVTKDlg::load(QString fname)
{
    if (fname.isEmpty())
    {
        xVLoadDlg dlg;
        if (dlg.exec()==QDialog::Accepted)
        {
            fname=dlg.currentDashboard();
        }
        else
        {
            emit KSignal(ST_WARN_MSG,new QString("import aborted by user"));
            return;
        }
    }
    else
    {
        // check if dashboard is already open
        int _nr=-1;
        int i=0;
        for (QList<xVDashBoard*>::iterator it=_dashboardLst.begin();it!=_dashboardLst.end();++it)
        {
            if ((*it)->_fileName==fname)
                _nr=i;
            ++i;
        }
        // then only activate it
        if (_nr!=-1)
        {
            emit KSignal(ST_WARN_MSG,new QString(QString("Dashboard %1 is already open").arg(fname)));
            emit KSignal(ST_ACTIVATE_DASHBOARD,new int(_nr));
        }
    }

    bool _abort=false;
    QFile f(fname);
    if (f.open(QFile::ReadOnly))
    {
        // detect if current dashboard can be overwritten
        if (_dashboardLst[_currentDashBoard]->_objLst.count()>2) emit KSignal(ST_CREATE_DASHBOARD);
        // clear all
        if (_dashboardLst[_currentDashBoard]->_objLst.count()>0)
        {
            xVAbstractBaseObj* pItem=_dashboardLst[_currentDashBoard]->_objLst.first();
            while (pItem && _dashboardLst[_currentDashBoard]->_objLst.count()>0)
            {
                _dashboardLst[_currentDashBoard]->_objLst.removeOne(pItem);
                removeObject(pItem);
                _dashboardLst[_currentDashBoard]->_objLst.count()>0 ? pItem = _dashboardLst[_currentDashBoard]->_objLst.first() : pItem=nullptr;
            }
        }

        QFileInfo info(fname);
        _dashboardLst[_currentDashBoard]->_fileName = info.absolutePath();
        _dashboardLst[_currentDashBoard]->_name = info.path().section("/",-1,-1);
        _dashboardLst[_currentDashBoard]->_boundToRealFileName = true;
        _dashboardLst[_currentDashBoard]->_lastAutoSaved = QDateTime::currentDateTime();
        _dashboardLst[_currentDashBoard]->_lastSaved = _dashboardLst[_currentDashBoard]->_lastAutoSaved;
        _settings["last dashboard file"]._value = QVariant::fromValue(xFileName(xFileName::FN_OUTPUT_FILE,info.absoluteFilePath()));
        emit KSignal(ST_SETTINGS_MODIFIED);

        QDataStream d(&f);
        QPixmap pix; d >> pix;
        QStringList header; d >> _dashboardLst[_currentDashBoard]->_header;
        d >> _dashboardLst[_currentDashBoard]->_explicit;
        QString hStr;d >> hStr;
        quint16 _count; d >> _count;
        ui->pProg->setRange(0,_count-1);
        quint16 _baseObjCount; d >> _baseObjCount;
        // load base objects
        for (int i=0;i<_baseObjCount && !_abort;++i) {
            quint64 _pos = d.device()->pos();
            quint32 _type;d >> _type;
            emit KSignal(ST_MSG,new QString(QString("loading %1 %2").arg(_pos,6,16,QChar('0')).arg(_type)));
            xVObj_Basics* pObj=nullptr;
            switch ((xVO_TYPE)_type)
            {
            case xVOT_MEASUREMENT_REGION: pObj = new xVMeasurementRegion(d);break;
            case xVOT_CLIPPING_BOX:     pObj = new xVBoxObj(d);break;
            case xVOT_CLIPPING_PLANE:   pObj = new xVPlaneObj(d);break;
            case xVOT_POLYGONIZATION:   pObj = new xVPolygonizationFilterObj(d);break;
            case xVOT_MORPH_FILTER:     pObj = new xVMorphFilterObj(d);break;
            case xVOT_LSI_FILTER:       pObj = new xVLSIFilterObj(d);break;
            case xVOT_FUZZIFICATION:    pObj = new xVFuzzificationObj(d);break;
            case xVOT_FUZZY_RULES:      pObj = new xVFuzzyRuleBaseObj(d);break;
            case xVOT_DEFUZZIFICATION:  pObj = new xVDefuzzificationObj(d);break;
            case xVOT_GRAPH:            pObj = new xVGraphObj(d);break;
            case xVOT_EXPORT:           pObj = new xVExportObj(d);break;
            case xVOT_IMAGE:            pObj = new xVImageObj(d);break;
            case xVOT_IMAGE_STACK:      pObj = new xVImageStackObj(d);break;
            case xVOT_REPORT:           pObj = new xVReportObj(d);break;
            case xVOT_CALL_DASHBOARD:   pObj = new xVCallDashboardObj(d);break;
            case xVOT_PYLON_CAM_PROP:   pObj = new xVCamPropObj(d);break;
            case xVOT_PYLON_CAMERA:     pObj = new xVCamObj(d);break;
            case xVOT_CVS:              pObj = new xVImportCVSObj(d);break;
            case xVOT_ARDUINO_COM:      pObj = new xVArduinoComObj(d);break;
            case xVOT_ARDUINO_CONNECT:  pObj = new xVArduinoConnectObj(d);break;
            case xVOT_WAIT:             pObj = new xVWaitObj(d);break;
            case xVOT_IF:               pObj = new xVIFObj(d);break;
            case xVOT_END:              pObj = new xVEndObj(d);break;
            case xVOT_START:            pObj = new xVStartObj(d);break;
            case xVOT_VOLUME:           pObj = new xVVolObj(d);break;
            case xVOT_MESH:             pObj = new xVPolyObj(d);break;
            case xVOT_2D_VIEW:          pObj = new xV2DVisObj(d);break;
            case xVOT_3D_VIEW:          pObj = new xV3DVisObj(d);break;
            case xVOT_VOLUME_VIS_PROP:  pObj = new xVVolumeVisPropObj(d);     break;
            case xVOT_MESH_VIS_PROP:    pObj = new xVMeshVisPropObj(d);       break;
            case xVOT_USER_TABLE_DLG:   pObj = new xVUserTableImportDlgObj(d);break;
            case xVOT_VAR_DEFINITION:   pObj = new xVVarDefinitionObj(d);     break;
            case xVOT_TRAFFIC_LIGHT:    pObj = new xVTrafficLightObj(d);      break;
            case xVOT_MATH:             pObj = new xVMathObj(d);              break;
            default:
                emit KSignal(ST_ERROR_MSG,new QString("invalid object type found. import aborted"));
                _abort=true;
                continue;
            }

            QPointF pnt; d >> pnt;

            if (pObj)
            {
                _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pObj->item());
                //connect(pObj,SIGNAL(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)));
                emit KSignal(ST_ADD_OBJECT,pObj);
                pObj->item()->setPos(pnt);
            }
        }

        // convert all ids of the connectors to real references
        for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
        {
            xVObj_Basics* pVObj=dynamic_cast<xVObj_Basics*>(*it);
            if (pVObj)
            {
                for (QList<xConnector*>::iterator it2=pVObj->connectorLst()->begin();it2!=pVObj->connectorLst()->end();++it2)
                {
                    while ((*it2)->objIDs()->count()>0)
                    {
                        QString vObjId = (*it2)->objIDs()->takeFirst();
                        xVObj_Basics* pCurrentVObj = dynamic_cast<xVObj_Basics*>(getObjById(vObjId));
                        if (pCurrentVObj)
                            (*it2)->addConObject(pCurrentVObj);
                    }
                }
                // map all parameter ids
                pVObj->translateObjIDsToPtr();
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
            _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pConObj->item());
            emit KSignal(ST_ADD_OBJECT,pConObj);
        }

        emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);
        emit KSignal(ST_DASHBOARD_INFO_MODIFIED,_dashboardLst[_currentDashBoard]);

        f.close();
    }
    else
    {
        clear(false);
        emit KSignal(ST_WARN_MSG,new QString("couldn't open dashboard file"));
    }
}
