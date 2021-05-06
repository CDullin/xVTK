#ifndef XVCLOCK_H
#define XVCLOCK_H

#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QDateTime>
#include <QPainter>

class xVClock:public QLabel
{
    Q_OBJECT
public:
    xVClock(QWidget* parent=nullptr);
    void setPixmap(const QPixmap&);
protected:
    QPixmap generatePixmap();
    virtual void resizeEvent(QResizeEvent *) override;
protected slots:
    void timeout();
private:
    QPixmap pix;
};

#endif // XVCLOCK_H
