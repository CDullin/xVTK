#include "xVTable.h"
#include <QTextStream>

QDataStream &operator<<(QDataStream &out, const xVTablePtr &myObj){
    //out << myObj.absoluteFilePath();
    return out;}
QDataStream &operator>>(QDataStream &in, xVTablePtr &myObj){
    QString s;
    //in >> s;myObj.setFile(s);
    return in;}

xVTable::xVTable():QObject(){}
xVTable::xVTable(const xVTable&):QObject(){}
xVTable::xVTable(QDataStream &):QObject(){}
int xVTable::rowCount(){
    return _table.count();
}
int xVTable::colCount(){
    int c=0;
    for (long r=0;r<_table.count();++r)
        c=std::max(c,_table[r].count());
    return c;
}
void xVTable::clear()
{
    _table.clear();
}
void xVTable::insert(int r, int c, QVariant v)
{
    while (_table.count()<=r)
        _table.append(QVector<QVariant>());
    while (_table[r].count()<=c)
        _table[r].append(QVariant());
    _table[r][c]=v;
}
QVariant xVTable::guessDataType(QString s)
{
    QVariant res;
    bool ok;
    double dval=s.toDouble(&ok);
    if (ok) res=dval;
    else
    {
        int ival=s.toInt(&ok);
        if (ok) res=ival;
        else
            res=s;
    }
    return res;
}
QVariant xVTable::item(int r, int c)
{
    QVariant res="";
    if (r>=0 && r<_table.count() && c>=0 && c<_table[r].count())
        res=_table[r][c];
    return res;
}
void xVTable::fromCVS()
{
    QFile f(_referenceFile);
    if (f.open(QFile::ReadOnly))
    {
        _header.clear();
        QTextStream t(&f);
        QString line;
        long id=0;
        for (int i=0;i<_importParam._headerSkipLines;++i) t.readLine();
        while (!t.atEnd())
        {
            line = t.readLine();
            QStringList val=line.split(_importParam._separator);
            for (int j=0;j<_importParam._skipEveryNLines;++j) t.readLine();

            if (_importParam._dataOrderedInColumns)
            {
                // header are the first N rows
                if (id<_importParam._useFirstNLinesAsLabel)
                {
                    if (id==0)
                        for (int k=0;k<val.count();++k) _header.append(QStringList() << val[k]);
                    else
                        for (int k=0;k<val.count();++k) _header[k] << val[k];
                }
                else
                {
                    for (int k=0;k<val.count();++k)
                        insert(id,k,guessDataType(val[k]));
                }
                ++id;
            }
            else
            {
                // headaer are the first N cols
            }
        }

        f.close();
    }
    else emit KSignal(ST_ERROR_MSG,new QString("unable to open file"));
}
