#include "xvEvalCondition.h"
#include "xVObjects.h"

#include <stdlib.h>

using namespace std;

QDataStream &operator<<(QDataStream &out, const xVEvalCondition &myObj){
    myObj.save(out);
return out;}
QDataStream &operator>>(QDataStream &in, xVEvalCondition &myObj){
    QString con;in >> con;
    myObj.setString(con);
    return in;}

xVEvalCondition::xVEvalCondition(const QString& s,QObject *parent) : QObject(parent)
{
    setString(s);
}

xVEvalCondition::xVEvalCondition(QDataStream &d) : QObject()
{
    d >> _condition;
}

void xVEvalCondition::save(QDataStream &d) const
{
    d << _condition;
}

QVariant xVEvalCondition::_neg(QVariant a)
{
    QVariant res;
    if (a.type()==QVariant::Bool) res=!a.toBool();
    if (a.type()==QVariant::LongLong) res=-a.toLongLong();
    if (a.type()==QVariant::Double) res=-a.toDouble();
    return res;
}

QVariant xVEvalCondition::_and(QVariant a, QVariant b)
{
    QVariant res;

    if (a.type()==QVariant::Bool && b.type()==QVariant::Bool) res=a.toBool() && b.toBool();
    if (a.type()!=QVariant::Bool || b.type()!=QVariant::Bool) res=a.toLongLong() & b.toLongLong();

    return res;
}

QVariant xVEvalCondition::_or(QVariant a, QVariant b)
{
    QVariant res;

    if (a.type()==QVariant::Bool && b.type()==QVariant::Bool) res=a.toBool() || b.toBool();
    if (a.type()!=QVariant::Bool || b.type()!=QVariant::Bool) res=a.toLongLong() | b.toLongLong();

    return res;
}

QVariant xVEvalCondition::_equal(QVariant a, QVariant b)
{
    QVariant res;

    if (a.type()==QVariant::Bool && b.type()==QVariant::Bool) res=a.toBool() == b.toBool();
    if (a.type()!=QVariant::Bool || b.type()!=QVariant::Bool) res=a.toLongLong() == b.toLongLong();

    return res;
}

QVariant xVEvalCondition::_notEqual(QVariant a, QVariant b)
{
    return _neg(_equal(a,b));
}

QVariant xVEvalCondition::_xor(QVariant a, QVariant b)
{
    QVariant res;

    if (a.type()==QVariant::Bool && b.type()==QVariant::Bool) res=(!a.toBool()) != (!b.toBool());
    if (a.type()!=QVariant::Bool || b.type()!=QVariant::Bool) res=a.toLongLong() ^ b.toLongLong();

    return res;
}

QVariant xVEvalCondition::_nand(QVariant a, QVariant b)
{
    return _neg(_and(a,b));
}

QVariant xVEvalCondition::_nor(QVariant a, QVariant b)
{
    return _neg(_or(a,b));
}

QVariant xVEvalCondition::_lshift(QVariant a,QVariant b)
{
    QVariant res = a.toLongLong() << b.toLongLong();
    return res;
}

QVariant xVEvalCondition::_rshift(QVariant a,QVariant b)
{
    QVariant res = a.toLongLong() >> b.toLongLong();
    return res;
}

QVariant xVEvalCondition::_plus(QVariant a,QVariant b)
{
    if (a.type()==QVariant::Double || b.type()==QVariant::Double) return a.toDouble()+b.toDouble();
    if (a.type()==QVariant::LongLong || b.type()==QVariant::LongLong) return a.toLongLong()+b.toLongLong();
    return _or(a,b);
}

QVariant xVEvalCondition::_minus(QVariant a,QVariant b)
{
    if (a.type()==QVariant::Double || b.type()==QVariant::Double) return a.toDouble()-b.toDouble();
    if (a.type()==QVariant::LongLong || b.type()==QVariant::LongLong) return a.toLongLong()-b.toLongLong();
    return _nand(a,b);
}

QVariant xVEvalCondition::_mul(QVariant a,QVariant b)
{
    if (a.type()==QVariant::Double || b.type()==QVariant::Double) return a.toDouble()*b.toDouble();
    if (a.type()==QVariant::LongLong || b.type()==QVariant::LongLong) return a.toLongLong()*b.toLongLong();
    return _and(a,b);
}

QVariant xVEvalCondition::_div(QVariant a,QVariant b)
{
    if (a.type()==QVariant::Double || b.type()==QVariant::Double) return a.toDouble()/b.toDouble();
    if (a.type()==QVariant::LongLong || b.type()==QVariant::LongLong) return a.toLongLong()/b.toLongLong();
    return _equal(a,b);
}

QVariant xVEvalCondition::_pow(QVariant a,QVariant b)
{
    if (a.type()==QVariant::Double || b.type()==QVariant::Double) return pow(a.toDouble(),b.toDouble());
    if (a.type()==QVariant::LongLong || b.type()==QVariant::LongLong) return pow(a.toLongLong(),b.toLongLong());
    return _xor(a,b);
}

QVariant xVEvalCondition::_mod(QVariant a,QVariant b)
{
    return a.toLongLong() % b.toLongLong();
}

QVariant xVEvalCondition::_radianOf(QVariant v)
{
    QVariant res;
    switch (_mode)
    {
    case TM_DEG : res = v.toDouble()/180.0*M_PI;break;
    case TM_RAD : res = v.toDouble();break;
    case TM_NRE : res = v.toDouble()/200.0*M_PI;break;
    }
    return res;
}

QVariant xVEvalCondition::_angleOfRadian(QVariant v)
{
    QVariant res;
    switch (_mode)
    {
    case TM_DEG : res = v.toDouble()/M_PI*180.0;break;
    case 1 : res = v.toDouble();break;
    case 2 : res = v.toDouble()/M_PI*200.0;break;
    }
    return res;
}

QVariant xVEvalCondition::_sin(QVariant a){return sin(a.toDouble());}
QVariant xVEvalCondition::_cos(QVariant a){return cos(a.toDouble());}
QVariant xVEvalCondition::_tan(QVariant a){return tan(a.toDouble());}
QVariant xVEvalCondition::_asin(QVariant a){return asin(a.toDouble());}
QVariant xVEvalCondition::_acos(QVariant a){return acos(a.toDouble());}
QVariant xVEvalCondition::_atan(QVariant a){return atan(a.toDouble());}
QVariant xVEvalCondition::_sqrt(QVariant a){return sqrt(a.toDouble());}

QVariant xVEvalCondition::calc_rec(QString txt)
{
    //ui->pEquationBrowser->append(txt);
    QVariant res=0.0;
    // split for brakets
    // split for *,/
    // split for +,-
    bool _jmp = false;

    if (txt.count()>0 && (txt[0]=="!" || txt[0]=="-")) return _neg(calc_rec(txt.right(txt.length()-1)));

    if (txt.contains('('))
    {
        _jmp = true;
        // find most left
        int idl = txt.indexOf(QString("("));
        int idr = txt.lastIndexOf(QString(")"));

        QVariant v=calc_rec(txt.mid(idl+1,idr-idl-1));
        if (v.type()==QVariant::Bool)
            txt = txt.replace(idl,idr-idl+1,QString("[bool|%1]").arg(v.toBool()));
        if (v.type()==QVariant::Double)
            txt = txt.replace(idl,idr-idl+1,QString("[real|%1]").arg(v.toReal()));
        if (v.type()==QVariant::LongLong)
            txt = txt.replace(idl,idr-idl+1,QString("[int|%1]").arg(v.toLongLong()));
    }

    QStringList opPattern={"and","or","nand","nor","xor","neg","<<",">>","<",">","<=",">=","!=","==","+","-","*","/","^","%"};
    long id=-1;
    for (int i=0;i<opPattern.count();++i)
        if (txt.contains(opPattern[i]))
            id=i;
    if (id!=-1)
    {
        _jmp = true;
        QVariant vl=calc_rec(txt.section(opPattern[id],0,0).simplified());
        QVariant vr=calc_rec(txt.right(txt.length()-txt.indexOf(opPattern[id])-opPattern[id].length()).simplified());

        switch (id)
        {
        case 0 : // and
            return _and(vl,vr);
            break;
        case 1 : // or
            return _or(vl,vr);
            break;
        case 2 : // nand
            return _nand(vl,vr);
            break;
        case 3 : // nor
            return _nor(vl,vr);
            break;
        case 4 : // xor
            return _xor(vl,vr);
            break;
        case 5 : // neg
            return _neg(vr);
            break;
        case 6 : // <<
            return _lshift(vl,vr);
            break;
        case 7 : // >>
            return _rshift(vl,vr);
            break;
        case 8 : // <
            return vl.toDouble() < vr.toDouble();
            break;
        case 9 : // >
            return vl.toDouble() > vr.toDouble();
            break;
        case 10 : // <=
            return vl.toDouble() <= vr.toDouble();
            break;
        case 11 : // >=
            return vl.toDouble() >= vr.toDouble();
            break;
        case 12 : // !=
            return _neg(_equal(vl,vr));
            break;
        case 13 : // ==
            return _equal(vl,vr);
            break;
        case 14 : // +
            return _plus(vl,vr);
            break;
        case 15 : // -
            return _minus(vl,vr);
            break;
        case 16 : // *
            return _mul(vl,vr);
            break;
        case 17 : // /
            return _div(vl,vr);
            break;
        case 18 : // ^
            return _pow(vl,vr);
            break;
        case 19 : // %
            return _mod(vl,vr);
            break;
        }
    }

    // functions
    const char *pattern[]={"asin","acos","atan","sin","cos","tan","√","sqr","exp"};
    int befNr=-1;
    for (int i=0;i<9;++i)
        if (txt.indexOf(pattern[i])==0)
            befNr=i;
    if (befNr!=-1)
    {
        _jmp=true;
        txt=txt.right(txt.length()-QString(pattern[befNr]).length());
        switch (befNr)
        {
        case 0 : return _angleOfRadian(_asin(calc_rec(txt)));break;
        case 1 : return _angleOfRadian(_acos(calc_rec(txt)));break;
        case 2 : return _angleOfRadian(_atan(calc_rec(txt)));break;
        case 3 : return _sin(_radianOf(calc_rec(txt)));break;
        case 4 : return _cos(_radianOf(calc_rec(txt)));break;
        case 5 : return _tan(_radianOf(calc_rec(txt)));break;
        case 6 : return _sqrt(calc_rec(txt));break;
        case 7 : return _pow(calc_rec(txt),2);break;
        case 8 : return _pow(M_E,calc_rec(txt));break;
        }
    }
/*
    // place holder
    const char* pattern_ph[]={"M","π"};
    befNr=-1;
    for (int i=0;i<2;++i)
        if (txt.indexOf(pattern_ph[i])==0)
            befNr=i;
    if (befNr!=-1)
    {
        switch (befNr)
        {
        case 1 : txt.replace(0,1,QString("%1").arg(M_PI,0,'f',12));break;
        case 0 : txt.replace(0,2,QString("%1").arg(_memory[txt.mid(1,1).toInt()]));break;
        }
    }
*/
    if (!_jmp)
    {
        bool ok;
        double val=txt.toDouble(&ok);
        if (!ok)
        {
            //should be [dtype|value]
            txt=txt.mid(1,txt.length()-2);
            QString dtype=txt.section("|",0,0);
            QString value=txt.section("|",1,1);
            QStringList dtypePattern = {"bool","int","real"};
            switch (dtypePattern.indexOf(dtype))
            {
            case 0 : res = QVariant((bool)value.toInt());break;
            case 1 : res = QVariant((long long)value.toLongLong());break;
            case 2 : res = QVariant((double)value.toDouble());break;
            }
        }
        else res=val;
    }
    return res;
}

QString varBlock(QString s)
{
    QString res="";
    int leftPos = s.indexOf("{");
    int rightPos = s.indexOf("}");

    if (leftPos>-1 && rightPos>-1)
        res = s.mid(leftPos,rightPos-leftPos+1);
    return res;
}

long hex2int(QString s)
{
    QString s2=s.right(s.length()-2);
    bool ok;
    return s2.toLong(&ok,16);
}

long oct2int(QString s)
{
    QString s2=s.right(s.length()-2);
    bool ok;
    return s2.toLong(&ok,8);
}

long bin2int(QString s)
{
    QString s2=s.right(s.length()-2);
    bool ok;
    return s2.toLong(&ok,2);
}

bool xVEvalCondition::isTrue(const QString & con)
{
    xVEvalCondition condition(con);
    return condition.toBool();
}

QVariant xVEvalCondition::resultOf(const QString& con)
{
    xVEvalCondition condition(con);
    return condition.result();
}

void xVEvalCondition::update()
{
    QString _cpy=_condition;

    // replace all conditionals (isBool) with their results
    const char* conditional_pattern[]={"isBool","isInt","isReal","isString","contains"};
    long _conditionPatternCount = 5;
    // syntax is supposed to be {var|pObjPtr}.conditional
    // syntax is supposed to be {var|pObjPtr}.contains("name")

    bool _reCheck=true;
    while (_reCheck)
    {
        _reCheck = false;
        int _foundPos = -1;
        int _patternNr = -1;
        for (int i=0;i<_conditionPatternCount && _foundPos==-1;++i)
        {
            _foundPos = _cpy.indexOf(conditional_pattern[i]);
            if (_foundPos!=-1) _patternNr=i;
        }

        if (_patternNr>-1)
        {
            _reCheck = true;
            QString secondParam = _cpy.right(_cpy.length()-_foundPos-QString(conditional_pattern[_patternNr]).length());
            secondParam = secondParam.section(")",0,0);
            secondParam = secondParam.mid(2,secondParam.length()-3);
            QString s = _cpy.left(_foundPos-2).section(" ",-1,-1);
            s = s.right(s.length()-1);
            QString varName = s.section("|",0,0);
            QString objPtr = s.section("|",1,1);
            xVObj_Basics* pVObj=(xVObj_Basics*)objPtr.toLong();
            xPROP_TYPE prop;

            bool _paramFound = false;
            if (pVObj)
            {
                if (pVObj->paramMap()->contains(varName))
                {
                    prop=(*pVObj->paramMap())[varName];
                    _paramFound = true;
                }
            }
            else
            {
                if (::_globalNameSpace.contains(varName))
                {
                    prop=::_globalNameSpace[varName];
                    _paramFound = true;
                }
            }

            bool _conditionEval = false;
            if (_paramFound)
            {
                switch (_patternNr)
                {
                case 0 : _conditionEval = prop._value.type()==QVariant::Bool;break;
                case 1 : _conditionEval =
                            prop._value.type()==QVariant::Int ||
                            prop._value.type()==QVariant::UInt ||
                            prop._value.type()==QVariant::LongLong ||
                            prop._value.type()==QVariant::ULongLong ||
                            prop._value.userType()==QMetaType::type("xLimitedInt");
                    break;
                case 2 : _conditionEval = prop._value.canConvert(QVariant::Double) ||
                            prop._value.userType()==QMetaType::type("xLimitedDouble");
                    break;
                case 3 : _conditionEval = prop._value.type()==QVariant::String ||
                            prop._value.userType()==QMetaType::type("QFileInfo");
                    break;
                case 4 : _conditionEval = (prop._value.type()==QVariant::String && prop._value.toString().contains(secondParam)) ||
                            (prop._value.userType()==QMetaType::type("QFileInfo") && prop._value.value<QFileInfo>().absoluteFilePath().contains(secondParam));break;
                }

            }
            // replace the whole thing
            long leftSpace = _cpy.lastIndexOf(" ",_foundPos);
            long rightSpace = _cpy.indexOf(" ",_foundPos);
            if (rightSpace==-1) rightSpace=_cpy.length();
            _cpy = _cpy.replace(leftSpace+1,rightSpace-leftSpace-1,QString("[bool|%1]").arg((int)_conditionEval));
        }
    }

    // replace all remaining variables with their values
    QString varBlk=varBlock(_cpy);
    while (!varBlk.isEmpty())
    {
        QString s=varBlk.mid(1,varBlk.length()-2);
        QString varName = s.section("|",0,0);
        QString objPtr = s.section("|",1,1);
        xVObj_Basics* pVObj=(xVObj_Basics*)objPtr.toLong();
        xPROP_TYPE prop;
        QString replacement="";

        bool _paramFound = false;
        if (pVObj)
        {
            if (pVObj->paramMap()->contains(varName))
            {
                prop=(*pVObj->paramMap())[varName];
                _paramFound = true;
            }
        }
        else
        {
            if (::_globalNameSpace.contains(varName))
            {
                prop=::_globalNameSpace[varName];
                _paramFound = true;
            }
        }

        if (!_paramFound) prop._value=true;
        switch (prop._value.type())
        {
        case QVariant::Bool: replacement=QString("[bool|%1]").arg(prop._value.toBool());break;
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::UInt:
        case QVariant::Int: replacement=QString("[int|%1]").arg(prop._value.toInt());break;
        case QMetaType::Float:
        case QVariant::Double: replacement=QString("[real|%1]").arg(prop._value.toDouble());break;
        case QVariant::String: replacement=QString("[string|%1]").arg(prop._value.toString());break;
        case QVariant::UserType:
            if (prop._value.userType()==QMetaType::type("QFileInfo"))
                replacement=QString("[string|%1]").arg(prop._value.value<QFileInfo>().absoluteFilePath());
            if (prop._value.userType()==QMetaType::type("xLimitedInt"))
                replacement=QString("[int|%1]").arg(prop._value.value<xLimitedInt>()._value);
            if (prop._value.userType()==QMetaType::type("xLimitedDouble"))
                replacement=QString("[int|%1]").arg(prop._value.value<xLimitedInt>()._value);
            break;
        }

        _cpy = _cpy.replace(_cpy.indexOf(varBlk),varBlk.length(),replacement);
        varBlk=varBlock(_cpy);
    }

    // replace all constants by [dtype|value]
    const char* constantTypePattern[] = {" 0x"," 0o"," 0b"};
    long _constantPatternCount = 3;

    _reCheck=true;
    while (_reCheck)
    {
        _reCheck = false;
        int _foundPos = -1;
        int _patternNr = -1;
        for (int i=0;i<_constantPatternCount && _foundPos==-1;++i)
        {
            _foundPos = _cpy.indexOf(constantTypePattern[i]);
            if (_foundPos!=-1) {
                _patternNr=i;
                _reCheck=true;
            }
        }

        if (_patternNr>-1)
        {
            int rightSpace = _cpy.indexOf(" ",_foundPos+1);
            if (rightSpace==-1) rightSpace=_cpy.length()-1;
            QString eval=_cpy.mid(_foundPos+1,_cpy.length()-rightSpace-_foundPos-1);
            QString replacement;
            switch (_patternNr)
            {
            case 0 : // hex
                replacement=QString("[int,%1]").arg(hex2int(eval));break;
            case 1 : // oct
                replacement=QString("[int,%1]").arg(oct2int(eval));break;
            case 2 : // bin
                replacement=QString("[int,%1]").arg(bin2int(eval));break;
            }
            _cpy = _cpy.replace(_foundPos+1,eval.length(),replacement);
        }
    }

    _result = calc_rec(_cpy).toBool();
}
