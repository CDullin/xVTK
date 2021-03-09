#ifndef XVFILEIMPORTDLGITEM_H
#define XVFILEIMPORTDLGITEM_H

#include <QWidget>
#include <QLineEdit>
#include <QFileInfo>
#include <QToolButton>

#include "xVTypes.h"

class xVFileImportDlgItem:public QWidget
{
    Q_OBJECT
public:
    enum FILE_MODE
    {
        READ_FILE,
        SAVE_FILE,
        OPEN_DIR
    };
    xVFileImportDlgItem(QWidget *parent=nullptr);
    void setMode(const FILE_MODE& m){_mode = m;}
    void setFilePath(const QString&);
    QFileInfo fileInfo();
protected slots:
    void browse();
signals:
    void modified();
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    QLineEdit *pLEdit;
    QToolButton *pBrowseTB;
    FILE_MODE _mode = READ_FILE;
    QFileInfo _currentInfo;
};

#endif // XVFILEIMPORTDLGITEM_H
