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
    void setRef(xVUserTableImportDlgObj*);
    void setRef(xVVarDefinitionObj*);
signals:
    void modified();
    void KSignal(const SIG_TYPE& ,void *);
protected slots:
    void clicked_SLOT();
protected:
    xVUserTableImportDlgObj* pRefObj=nullptr;
    xVVarDefinitionObj* pVarDefObj=nullptr;
};

#endif // XVUSERTABLEDEFINITIONDLGITEM_H

