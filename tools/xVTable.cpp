#include "xVTable.h"
#include <QTextStream>

xVTable::xVTable():QObject(){}
xVTable::xVTable(const xVTable&):QObject(){}
xVTable::xVTable(QDataStream &):QObject(){}
int xVTable::rowCount(){
    return _table.count();
}
int xVTable::colCount(){
    if (rowCount()>0) return _table[0].count();
    else return 0;
}
void xVTable::clear()
{
    _table.clear();
}
void xVTable::fromCVS(xParamMap* pParamRef)
{
    if (pParamRef)
    {
        if ((*pParamRef).contains("file name"))
        {
            QFile f((*pParamRef)["file name"]._value.value<QFileInfo>().absoluteFilePath());
            if (f.open(QFile::ReadOnly))
            {
                QTextStream t(&f);
                QString line;

                long _lineId=0;
                while (!t.atEnd())
                {
                    line = t.readLine();




                    _lineId++;
                }

                f.close();
            }
            else emit KSignal(ST_ERROR_MSG,new QString("unable to open file"));
        }
        else emit KSignal(ST_ERROR_MSG,new QString("no file name found"));
    }
    else emit KSignal(ST_ERROR_MSG,new QString("empty reference in convert table from csv"));
}
