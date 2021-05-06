#include "xVHelpBrowser.h"

xVHelpBrowser::xVHelpBrowser(QWidget* parent):QTextBrowser(parent)
{
    setSource(QUrl("qrc:/manual/manual.html"));
}

void xVHelpBrowser::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type)
    {
    default:
        //handled
        break;
    }
}
