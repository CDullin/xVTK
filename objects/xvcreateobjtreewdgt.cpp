#include "xvcreateobjtreewdgt.h"
#include "xviconfactory.h"

xVCreateObjTreeWdgt::xVCreateObjTreeWdgt(QWidget* parent):QTreeWidget(parent)
{
    connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )),this,SLOT(itemDoubleClicked_SLOT(QTreeWidgetItem *, int )));
}

void xVCreateObjTreeWdgt::itemDoubleClicked_SLOT(QTreeWidgetItem *item, int column)
{
    if (item && item->data(1,Qt::UserRole).isValid())
    {
        emit KSignal(ST_ACTION_HOOK_TRIGGERED,new int(item->data(1,Qt::UserRole).toInt()));
    }
}

void xVCreateObjTreeWdgt::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type) {
    case ST_ADD_ACTION_HOOKS:
        {
        HOOKLstPtr pHookLst = (HOOKLstPtr)pData;
        generateHooksFromLst(pHookLst);
        }
        break;
    default:
        // no need to handle everything
        break;
    }
}

QTreeWidgetItem* xVCreateObjTreeWdgt::pickOrCreateParentItem(const QString& pattern)
{
    QTreeWidgetItem* pItem = nullptr;
    for (int i=0;i<topLevelItemCount();++i)
    {
        if (topLevelItem(i)->text(0)==pattern)
            pItem=topLevelItem(i);
    }
    if (!pItem)
    {
        pItem = new QTreeWidgetItem(QStringList() << pattern);
        pItem->setIcon(0,::_iconFactory.icon(pattern));
        addTopLevelItem(pItem);
        pItem->setExpanded(true);
    }
    return pItem;
}

void xVCreateObjTreeWdgt::generateHooksFromLst(HOOKLstPtr pHLst)
{
    for (QList <HOOK>::iterator it=pHLst->begin();it!=pHLst->end();++it) {
        QTreeWidgetItem* pParentItem=pickOrCreateParentItem((*it)._rubrik);
        QTreeWidgetItem* pItem=new QTreeWidgetItem(QStringList() << (*it)._name);
        pParentItem->addChild(pItem);
        pItem->setData(1,Qt::UserRole,QVariant((int)(*it)._objType));
    }
}
