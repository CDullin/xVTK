#ifndef XVPROPWDGT_H
#define XVPROPWDGT_H

#include <QWidget>
#include "xVObjects.h"
#include "xVAbstractBaseObj.h"
#include <QTableWidgetItem>
#include <QFileInfo>
#include <QMetaProperty>

namespace Ui {
class xVPropWdgt;
}

class xVPropWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit xVPropWdgt(QWidget *parent = nullptr);
    ~xVPropWdgt();
    xVAbstractBaseObj* activeObj(){return pCurrentObj;}
    void installEventFilter(QObject*);

public slots:
    void objSelected(xVAbstractBaseObj*);
    void KSlot(const SIG_TYPE& t,void *pData=nullptr);
    void parameterModified();

protected slots:
    void deleteObj();
    void displayPreview();
    void connectionSelectionChanged();
    void deleteConnection();
    void moveConnectionUp();
    void moveConnectionDown();
    void identifyConnections(bool);
    void activateNext();
    void activatePrev();

protected:
    void updateParameterTable(xVAbstractBaseObj* pObj);
    void updateOutputConnectionList(xVAbstractBaseObj* pObj);

signals:
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);

private:
    Ui::xVPropWdgt *ui;
    xVAbstractBaseObj *pCurrentObj=nullptr;
    xConnector *pCurrentOutputConnector=nullptr;
    QTableWidget *pPropTable=nullptr;
    QList <QGraphicsSimpleTextItem*> _identifierLst;
};

#endif // XVPROPWDGT_H
