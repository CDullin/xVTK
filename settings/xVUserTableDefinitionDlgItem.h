#ifndef XVUSERTABLEDEFINITIONDLGITEM_H
#define XVUSERTABLEDEFINITIONDLGITEM_H

#include <QObject>
#include <QToolButton>
#include "xVObjects.h"
#include "xVTypes.h"
#include "xVObjectTypes.h"

class xVUserTableDefinitionDlgItem : public QToolButton
{
    Q_OBJECT
public:
    xVUserTableDefinitionDlgItem(QWidget* parent=nullptr);
    xParamMap paramMap();
    void setMap(xParamMap m,xVUserTableImportDlgObj*);
    void setMap(xParamMap m,xVVarDefinitionObj*);
signals:
    void modified();
    void KSignal(const SIG_TYPE& ,void *);
protected slots:
    void clicked_SLOT();
protected:
    xParamMap _paramMap;
    xVUserTableImportDlgObj* pRefObj=nullptr;
    xVVarDefinitionObj* pVarDefObj=nullptr;
};

#endif // XVUSERTABLEDEFINITIONDLGITEM_H

