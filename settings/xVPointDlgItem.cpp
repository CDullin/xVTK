#include "xVPointDlgItem.h"

#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QLabel>

xVPointDlgItem::xVPointDlgItem(QWidget *parent):QWidget(parent)
{
    setGeometry(0,0,100,20);
    pSB0 = new QLineEdit(this);
    pSB0->setText("0");
    pSB0->setGeometry(0,0,50,20);
    pSB0->setAlignment(Qt::AlignRight);
    pSB0->setValidator(new QDoubleValidator());
    pSB1 = new QLineEdit(this);
    pSB1->setAlignment(Qt::AlignRight);
    pSB1->setGeometry(50,0,50,20);
    pSB1->setText("0");
    pSB1->setValidator(new QDoubleValidator());

    connect(pSB0,SIGNAL(textEdited(const QString &)),this,SIGNAL(modified()));
    connect(pSB1,SIGNAL(textEdited(const QString &)),this,SIGNAL(modified()));

    QHBoxLayout *pVBox=new QHBoxLayout(this);
    pVBox->setContentsMargins(10,0,0,0);
    pVBox->addWidget(pSB0);
    pVBox->addWidget(new QLabel(" x "));
    pVBox->addWidget(pSB1);
}

void xVPointDlgItem::setPoint(const QPoint& p)
{
    pSB0->setText(QString("%1").arg(p.x()));
    pSB1->setText(QString("%1").arg(p.y()));
}

void xVPointDlgItem::setPoint(const QPointF& p)
{
    pSB0->setText(QString("%1").arg(p.x()));
    pSB1->setText(QString("%1").arg(p.y()));
}

QPointF xVPointDlgItem::point()
{
    return QPointF(pSB0->text().toFloat(),pSB1->text().toFloat());
}
