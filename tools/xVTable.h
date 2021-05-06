#ifndef XVTABLE_H
#define XVTABLE_H

#include <QDataStream>
#include "xVTypes.h"

//!
//! \brief The xVTable class
//! generic data table. Internally the data is always in columns
//!
class xVTable:public QObject
{
Q_OBJECT
public:

    struct xVTable_IMPORT_PARAM
    {
        QString _separator=",";
        int _headerSkipLines=0;
        int _skipEveryNLines=0;
        int _skipEveryNColumn=0;
        bool _dataOrderedInColumns=true;
        bool _autoDetectDataType=true;
        int _useFirstNLinesAsLabel=1;
        QString _decimalSeparator=".";
    };

    xVTable();
    xVTable(const xVTable&);
    xVTable(QDataStream &);
    int rowCount();
    int colCount();
    void clear();
    void fromCVS();
    QString file(){return _referenceFile;}
    void setFile(const QString& f){_referenceFile=f;}
    void insert(int r,int c,QVariant);
    QVariant guessDataType(QString);
    QVariant item(int,int);
    QVector<QStringList>* header(){return &_header;}
    xVTable_IMPORT_PARAM _importParam;

signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);

protected:
    QVector <QStringList> _header;
    QVector <QVector <QVariant>> _table;
    QString _referenceFile = "";

};

typedef xVTable* xVTablePtr;
Q_DECLARE_METATYPE(xVTablePtr);
QDataStream &operator<<(QDataStream &out, const xVTablePtr &myObj);
QDataStream &operator>>(QDataStream &in, xVTablePtr &myObj);

#endif // XVTABLE_H
