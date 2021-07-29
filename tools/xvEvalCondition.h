#ifndef XVEVALCONDITION_H
#define XVEVALCONDITION_H

#include <QObject>
#include "xVTypes.h"

/* evaluations equations to a bool
 *
 * supported operations "and","or","nand","nor","xor","neg","<<",">>","<",">","<=",">=","!=","==","+","-","*","/","^","%"
 * supported functions  "asin","acos","atan","sin","cos","tan","√","sqr","exp"
 * supported constant types int, double, 0x00 [hex], 0o00 [oct], 0b00 [bin]
 * supported constant
 *
 * C_PI     = 3.14159
 * C_E      =
 * C_H
 *
 * supported conditions isBool, isInt, isDouble, isString, contains
 *
 * example: {Var1|1902920} - {Var2|0} == 0xFF
 *
 * variable Var1 taken form parameter map of the object at the memomory location 1902920
 * variable Var2 taken from global namespace
 *
 * CD 2021-03-05
 */

class xVEvalCondition : public QObject
{
    Q_OBJECT
public:

    enum TRIG_MODE
    {
        TM_DEG,
        TM_RAD,
        TM_NRE
    };

    xVEvalCondition(const QString& str="",QObject *parent = nullptr);
    xVEvalCondition(QDataStream &d);
    xVEvalCondition(const xVEvalCondition& other):QObject() {_condition=other._condition;_mode=other._mode;_result=other._result;}

    void save(QDataStream& d) const;
    bool toBool(){update();return _result.toBool();};
    QVariant result(){update();return _result;}
    void setString(const QString& s){_condition=s;}
    void setTrigMode(const TRIG_MODE& m){_mode=m;}
    QString equation(){return _condition;}
    void evaluate(){update();}

    static bool isTrue(const QString&);
    static QVariant resultOf(const QString&);

    static QVariant _neg(QVariant a);
    static QVariant _and(QVariant a, QVariant b);
    static QVariant _or(QVariant a, QVariant b);
    static QVariant _equal(QVariant a, QVariant b);
    static QVariant _notEqual(QVariant a, QVariant b);
    static QVariant _xor(QVariant a, QVariant b);
    static QVariant _nand(QVariant a, QVariant b);
    static QVariant _nor(QVariant a, QVariant b);
    static QVariant _lshift(QVariant a,QVariant b);
    static QVariant _rshift(QVariant a,QVariant b);
    static QVariant _plus(QVariant a,QVariant b);
    static QVariant _minus(QVariant a,QVariant b);
    static QVariant _mul(QVariant a,QVariant b);
    static QVariant _div(QVariant a,QVariant b);
    static QVariant _pow(QVariant a,QVariant b);
    static QVariant _mod(QVariant a,QVariant b);
    static QVariant _sin(QVariant a);
    static QVariant _cos(QVariant a);
    static QVariant _tan(QVariant a);
    static QVariant _asin(QVariant a);
    static QVariant _acos(QVariant a);
    static QVariant _atan(QVariant a);
    static QVariant _sqrt(QVariant a);

    QVariant _radianOf(QVariant v);
    QVariant _angleOfRadian(QVariant v);

    void updateFromIDtoPtr(){_condition=translateFrmID(_condition);}

signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);

protected:    
    QVariant calc_rec(QString txt);
    void update();
    QString translateToID(QString) const;
    QString translateFrmID(QString) const;
    
    QString _condition;
    QVariant _result;
    TRIG_MODE _mode = TM_DEG;
};

Q_DECLARE_METATYPE(xVEvalCondition) ;
QDataStream &operator<<(QDataStream &out, const xVEvalCondition &myObj);
QDataStream &operator>>(QDataStream &in, xVEvalCondition &myObj);

#endif // XVEVALCONDITION_H
