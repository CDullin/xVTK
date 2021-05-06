#ifndef XVIMPORTCVSOBJ_H
#define XVIMPORTCVSOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenImpObj.h"
#include "xVCVSImportPreviewDlg.h"

//!
//! \brief The xVImportCVSObj class
//! is used to import *.cvs files based on the specific settings and convert them into a table in the global namespace
//!
class xVImportCVSObj : public xVGenImpObj
{
    Q_OBJECT
public:
    xVImportCVSObj(const QString&);
    xVImportCVSObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;
    virtual void openPreview() override;
protected:
    xVCVSImportPreviewDlg *pPrevDlg=nullptr;
};

#endif // XVIMPORTCVSOBJ_H
