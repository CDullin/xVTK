#include "xvvismainwin.h"
#include "ui_xvvismainwin.h"

xVVisMainWin::xVVisMainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xVVisMainWin)
{
    ui->setupUi(this);
}

xVVisMainWin::~xVVisMainWin()
{
    delete ui;
}

void xVVisMainWin::addWindowAction(QAction *pAction)
{
    ui->menuwindows->addAction(pAction);
}

void xVVisMainWin::removeWindowAction(QWidget* wdgt)
{
    if (!ui->menuwindows) return;
    QAction *pAction=nullptr;
    QList<QAction *> actionLst=ui->menuwindows->actions();
    for (QList<QAction *>::iterator it=actionLst.begin();
         it!=actionLst.end();
         ++it) {
        if ((*it)->property("widget").isValid() && (*it)->property("widget").value<void*>()==(void*)wdgt)
            pAction=*it;
    }
    if (pAction) {
        disconnect(pAction,0,0,0);
        ui->menuwindows->removeAction(pAction);
    }
}
