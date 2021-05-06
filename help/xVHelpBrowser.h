#ifndef XVHELPBROWSER_H
#define XVHELPBROWSER_H

#include "xVTypes.h"
#include <QTextBrowser>

class xVHelpBrowser:public QTextBrowser
{
    Q_OBJECT
public:
    xVHelpBrowser(QWidget *parent = nullptr);
protected slots:
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);
signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);
};

#endif // XVHELPBROWSER_H
