#ifndef XVUSERTABLEIMPORTDLGOBJ_H
#define XVUSERTABLEIMPORTDLGOBJ_H

#include "xVTypes.h"
#include "xVGenUserDlgObj.h"
#include <QDataStream>

class xVUserTableImportDlgObj:public xVGenUserDlgObj
{
    Q_OBJECT
public:
    xVUserTableImportDlgObj(const QString& txt="");
    xVUserTableImportDlgObj(QDataStream& d);
    virtual void save(QDataStream& d,bool _explicit=false) override;
    virtual void run() override;
};

#endif // XVUSERTABLEIMPORTDLGOBJ_H
