#include "xVAbstractBaseObj.h"
#include <QDateTime>
#include "xvEvalCondition.h"

xVAbstractBaseObj::xVAbstractBaseObj():QObject(){
    _selected=false;
    _id=QDateTime::currentDateTime().toString("yyMMddhhmmsszzz")+QString("_%1").arg(::_objIDCount++);}

void xVAbstractBaseObj::translateObjIDsToPtr()
{
    for (QMap<QString,xPROP_TYPE>::iterator it=_paramMp.begin();it!=_paramMp.end();++it)
    {
        if (!it->_subGrpIDStr.isEmpty() && it->pRefObj==nullptr)
        {
            it->pRefObj = objId2objPtr(it->_subGrpIDStr);
        }
        if (it->_value.userType()==QMetaType::type("xVEvalCondition"))
        {
            xVEvalCondition cond=it->_value.value<xVEvalCondition>();
            cond.updateFromIDtoPtr();
            it->_value=QVariant::fromValue<xVEvalCondition>(cond);
        }
        if (it->_value.userType()==QMetaType::type("xParamMap"))
        {
            xParamMap _2ndMp = it->_value.value<xParamMap>();
            for (QMap<QString,xPROP_TYPE>::iterator it2=_2ndMp.begin();it2!=_2ndMp.end();++it2)
            {
                if (!it2->_subGrpIDStr.isEmpty() && it2->pRefObj==nullptr)
                {
                    it2->pRefObj = objId2objPtr(it2->_subGrpIDStr);
                }
            }
            it->_value = QVariant::fromValue<xParamMap>(_2ndMp);
        }
    }
}

void xVAbstractBaseObj::KSlot(const SIG_TYPE&,void* data)
{

}

void xVAbstractBaseObj::save(QDataStream& d,bool _explicit)
{
    d << _selected;
    d << (quint16)_paramMp.count();
    for (QMap<QString,xPROP_TYPE>::iterator it=_paramMp.begin();it!=_paramMp.end();++it)
    {
        d << it.key();
        it->save(d);
    }
    d << _id;
}


xVAbstractBaseObj::xVAbstractBaseObj(QDataStream& d):QObject()
{
    quint64 _pos = d.device()->pos();
    d >> _selected;
    quint16 _count; d >> _count;
    for (int i=0;i<_count;++i)
    {
        QString k;d >> k;
        xPROP_TYPE prop(d);
        _paramMp[k]=prop;
    }
    d >> _id;
}

void xVAbstractBaseObj::setParamSelected(bool b){_selected=b;}
