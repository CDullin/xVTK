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
    xVTable();
    xVTable(const xVTable&);
    xVTable(QDataStream &);
    int rowCount();
    int colCount();
    void clear();
    void fromCVS(xParamMap*);

signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);

protected:
    QVector <QString> _header;
    QVector <QVector <QVariant>> _table;
};


#endif // XVTABLE_H
