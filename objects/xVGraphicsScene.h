#ifndef XVGRAPHICSSCENE_H
#define XVGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

//!
//! \brief The xVGraphicsScene class
//!

class xVGraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    xVGraphicsScene():QGraphicsScene(){}
signals:
    void dblClicked();
    void rDblClicked();
protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override
    {
        if (mouseEvent->button()==Qt::LeftButton)
            emit dblClicked();
        if (mouseEvent->button()==Qt::RightButton)
            emit rDblClicked();
        QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
    }
};

#endif // XVGRAPHICSSCENE_H
