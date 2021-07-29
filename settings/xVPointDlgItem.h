#ifndef XVPOINTDLGITEM_H
#define XVPOINTDLGITEM_H

#include <QLineEdit>
#include <QWidget>
#include "xVTypes.h"

class xVPointDlgItem:public QWidget
{
Q_OBJECT
public:
    xVPointDlgItem(QWidget *parent = nullptr);
    void setPoint(const QPoint& b);
    void setPoint(const QPointF& b);
    QPointF point();
signals:
    void modified();
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);
protected:
    //virtual void resizeEvent(QResizeEvent *event) override;
    QLineEdit *pSB0,*pSB1;
};

#endif // XVPOINTDLGITEM_H
