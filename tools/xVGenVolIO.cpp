#include "xVGenVolIO.h"

xVGenVolIO::xVGenVolIO(QObject* parent):QObject(parent)
{
    pProgObserver = new xVProgressObserver();
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
}
