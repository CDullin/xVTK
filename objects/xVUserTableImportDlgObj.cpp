#include "xVUserTableImportDlgObj.h"
#include "xvusertableruntimedlg.h"

xVUserTableImportDlgObj::xVUserTableImportDlgObj(const QString& txt):xVGenUserDlgObj(txt)
{
    _type = xVOT_USER_TABLE_DLG;
    _paramMp["title"]._id=1;
    _paramMp["title"]._value="param input dialog";
    _paramMp["parameter table"]._id=2;
    _paramMp["parameter table"]._value=QVariant::fromValue(xParamMap());
}
xVUserTableImportDlgObj::xVUserTableImportDlgObj(QDataStream& d):xVGenUserDlgObj(d)
{
    _type = xVOT_USER_TABLE_DLG;
}

void xVUserTableImportDlgObj::save(QDataStream& d,bool _explicit)
{
    xVGenUserDlgObj::save(d);
}

void xVUserTableImportDlgObj::run()
{
    xVGenUserDlgObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;
    setStatus(OS_RUNNING);
    // override values of the connected parameter object or the global namespace
    emit KSignal(ST_MSG,new QString("dialog started"));

    xVUserTableRuntimeDlg dlg;
    dlg.setTitle(_paramMp["title"]._value.toString());
    xParamMap map(_paramMp["parameter table"]._value.value<xParamMap>());
    dlg.setMap(&map,this);
    dlg.exec();
    _paramMp["parameter table"]._value=QVariant::fromValue<xParamMap>(map);
    for (QList<xConnector*>::iterator it=connectorLst()->begin();it!=connectorLst()->end();++it)
        if ((*it)->type()==xCT_OUTPUT)
            for (QList <xVObj_Basics*>::iterator it2=(*it)->connectedObjects()->begin();it2!=(*it)->connectedObjects()->end();++it2)
                for (xParamMap::iterator it3=map.begin();it3!=map.end();++it3)
                {
                    QString key=it3.key();
                    if ((*it2)->paramMap()->contains(it3.key()) && (*it3).pRefObj==(*it2))
                            (*(*it2)->paramMap())[it3.key()]=it3.value();
                }
    // override global namespace
    bool _globalNamespace_modified = false;
    for (xParamMap::iterator it3=map.begin();it3!=map.end();++it3)
    {
        QString key=it3.key();
        if (::_globalNameSpace.contains(it3.key()) && (*it3).pRefObj==nullptr)
        {
            ::_globalNameSpace[it3.key()]=it3.value();
            _globalNamespace_modified = true;
        }
    }
    if (_globalNamespace_modified) emit KSignal(ST_GLOBAL_NAMESPACE_MODIFIED);

    setStatus(OS_VALID);
}
