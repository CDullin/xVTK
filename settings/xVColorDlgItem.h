#ifndef XVCOLORDLGITEM_H
#define XVCOLORDLGITEM_H

#include <QLabel>
#include <QPixmap>
#include "xVTypes.h"

class xVColorDlgItem:public QLabel
{
    Q_OBJECT
public:
    xVColorDlgItem(QWidget *parent=nullptr);
    QColor color(){return _color;}
public slots:
    void setColor(const QColor&);
signals:
    void modified();
    void KSignal(const SIG_TYPE& t,void *pData=nullptr);
protected:
    QPixmap generatePixmap();
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    QColor _color;
};

#endif // XVCOLORDLGITEM_H
