#include "xvhistodlg.h"
#include <math.h>
#include <stdlib.h>
#include <QDoubleValidator>

using namespace std;


void xVHistoDlg::dblClickInAll(QPointF p)
{
    bool ok;
    switch (_mode)
    {
    case HDM_OPACITY:
    {
        QPointF pnt=mapPxFrmAllPixmapToGValAndIntensity(p,ok);
        opacityFuncRef->AddPoint(pnt.x(),pnt.y());
        createOpacityFunc();
        if (_paramMp["live mode"]._value.toBool()) emit modified("opacity function");
        pColorBarPixItem->setPixmap(generateColorBar(pColorBarPixItem->pixmap().width(),pColorBarPixItem->pixmap().height()));
    }
        break;
    }
}

void xVHistoDlg::killNode(xVHistoNodeItem* pNode)
{
    switch (_mode) {
    case HDM_OPACITY:
    {
        double val[4];
        opacityFuncRef->GetNodeValue(pNode->nr(),val);
        opacityFuncRef->RemovePoint(val[0]);
        createOpacityFunc();
        if (_paramMp["live mode"]._value.toBool()) emit modified("opacity function");
        pColorBarPixItem->setPixmap(generateColorBar(pColorBarPixItem->pixmap().width(),pColorBarPixItem->pixmap().height()));
    }
        break;
    case HDM_LUT:
    {
        double val[4];
        colorTransFuncRef->GetNodeValue(pNode->nr(),val);
        colorTransFuncRef->RemovePoint(val[0]);
        createColorFunc();
        if (_paramMp["live mode"]._value.toBool()) emit modified("color function");
        pColorBarPixItem->setPixmap(generateColorBar(pColorBarPixItem->pixmap().width(),pColorBarPixItem->pixmap().height()));
    }
        break;
    }
}

void xVHistoDlg::dblClickInZoom(QPointF p)
{

}

void xVHistoDlg::interactiveElementMoved(QPointF p,const int& type)
{
    double w = pHistoGV->width()*.8;
    double h = pHistoGV->height()*.8;
    pHistoGV->setSceneRect(0,0,w,h);
    switch (type)
    {
    case HLM_ZOOM_LEFT:
        if (pZoomLeft && pZoomRight && pZoomLeft->pos().x()>=pZoomRight->pos().x())
            pZoomLeft->setPos(pZoomRight->pos().x()-2,pZoomLeft->pos().y());
        break;
    case HLM_ZOOM_RIGHT:
        if (pZoomLeft && pZoomRight && pZoomRight->pos().x()<=pZoomLeft->pos().x())
            pZoomRight->setPos(pZoomLeft->pos().x()+2,pZoomRight->pos().y());
        break;
    }
    if (pZoomLeft && pZoomRight && (type==HLM_ZOOM_LEFT || type==HLM_ZOOM_RIGHT)) // updateZoom
    {
        bool ok;
        float lpos = pZoomLeft->pos().x();
        float rpos = pZoomRight->pos().x();
        QPointF lp=mapPxFrmAllPixmapToGValAndIntensity(QPointF(lpos,0),ok);
        QPointF rp=mapPxFrmAllPixmapToGValAndIntensity(QPointF(rpos,0),ok);
        if (pZoomHistoPixItem)
        {
            _zoom = generatePixmap(lp.x(),rp.x(),w*0.95,h*0.25);
            pZoomHistoPixItem->setPixmap(_zoom.pix);
            QPointF zoomPixBottomLeft = pZoomHistoPixItem->pos()+QPointF(0,pZoomHistoPixItem->pixmap().height());
            QPointF zoomPixBottomRight = pZoomHistoPixItem->pos()+QPointF(pZoomHistoPixItem->pixmap().width(),pZoomHistoPixItem->pixmap().height());
            pZoomConLeft->setLine(QLineF(pZoomLeft->mapToScene(QPointF(0,0)),zoomPixBottomLeft));
            pZoomConRight->setLine(QLineF(pZoomRight->mapToScene(QPointF(0,0)),zoomPixBottomRight));
            pHorZoomAxis->updateAxis();
            pVerZoomAxis->updateAxis();
        }

        switch (_mode)
        {
        case HDM_OPACITY : updateOpacityFunc();break;
        case HDM_LUT: updateColorFunc(); break;
        }                
    }
}
void xVHistoDlg::interactiveElementDblClicked(QPointF p, const int& type)
{
    if (!pInteractiveLEdit)
    {
        pInteractiveLEdit = new QLineEdit(this);
        pInteractiveLEdit->setValidator(new QDoubleValidator());
        pInteractiveLEdit->setAlignment(Qt::AlignRight);
        connect(pInteractiveLEdit,SIGNAL(returnPressed()),this,SLOT(interactiveLEditTriggered()));
    }
    pInteractiveLEdit->raise();
    pInteractiveLEdit->setFocus();

    bool ok;
    switch (type)
    {
    case HLM_ZOOM_LEFT:
    {
        QPointF pWdgt = mapFromGlobal(pHistoGV->viewport()->mapToGlobal(pHistoGV->mapFromScene(pZoomLeft->mapToScene(p))));
        pInteractiveLEdit->setGeometry(
                    max(0,min((int)pWdgt.x()-25,width()-25)),
                    max(0,min((int)pWdgt.y()-10,height()-20)),50,20);
        pInteractiveLEdit->setText(QString("%1").arg(mapPxFrmAllPixmapToGValAndIntensity(pZoomLeft->pos(),ok).x()));
        pInteractiveLEdit->show();
        _currentInteractiveObj = (HISTO_LINE_MODE)type;
    }
    break;
    case HLM_ZOOM_RIGHT:
    {
        QPointF pWdgt = mapFromGlobal(pHistoGV->viewport()->mapToGlobal(pHistoGV->mapFromScene(pZoomRight->mapToScene(p))));
        pInteractiveLEdit->setGeometry(
                    max(0,min((int)pWdgt.x()-25,width()-25)),
                    max(0,min((int)pWdgt.y()-10,height()-20)),50,20);
        pInteractiveLEdit->setText(QString("%1").arg(mapPxFrmAllPixmapToGValAndIntensity(pZoomRight->pos(),ok).x()));
        pInteractiveLEdit->show();
        _currentInteractiveObj = (HISTO_LINE_MODE)type;
    }
    break;
    }
}

void xVHistoDlg::interactiveLEditTriggered()
{
    double w = pHistoGV->width()*.8;
    double h = pHistoGV->height()*.8;
    pHistoGV->setSceneRect(0,0,w,h);

    if (pInteractiveLEdit)
    {
        bool ok;
        float lpos,rpos;

        switch (_currentInteractiveObj)
        {
        case HLM_ZOOM_LEFT:
        {
            lpos = mapGValToAllPixmap(pInteractiveLEdit->text().toFloat(),ok);
            rpos = pZoomRight->pos().x();
        }
            break;
        case HLM_ZOOM_RIGHT:
        {
            rpos = mapGValToAllPixmap(pInteractiveLEdit->text().toFloat(),ok);
            lpos = pZoomLeft->pos().x();
        }
            break;
        }
        if (ok && (_currentInteractiveObj==HLM_ZOOM_LEFT || _currentInteractiveObj==HLM_ZOOM_RIGHT))
        {
            QPointF lp=mapPxFrmAllPixmapToGValAndIntensity(QPointF(lpos,0),ok);
            QPointF rp=mapPxFrmAllPixmapToGValAndIntensity(QPointF(rpos,0),ok);
            if (pZoomHistoPixItem)
            {
                pZoomLeft->setPos(lpos,0);
                pZoomRight->setPos(rpos,0);
                _zoom = generatePixmap(lp.x(),rp.x(),w*0.95,h*0.3);
                pZoomHistoPixItem->setPixmap(_zoom.pix);
                QPointF zoomPixBottomLeft = pZoomHistoPixItem->pos()+QPointF(0,pZoomHistoPixItem->pixmap().height());
                QPointF zoomPixBottomRight = pZoomHistoPixItem->pos()+QPointF(pZoomHistoPixItem->pixmap().width(),pZoomHistoPixItem->pixmap().height());
                pZoomConLeft->setLine(QLineF(pZoomLeft->mapToScene(QPointF(0,0)),zoomPixBottomLeft));
                pZoomConRight->setLine(QLineF(pZoomRight->mapToScene(QPointF(0,0)),zoomPixBottomRight));
                pHorZoomAxis->updateAxis();
            }
        }
        pInteractiveLEdit->hide();
    }
}


float xVHistoDlg::mapGValToAllPixmap(float gval,bool& ok)
{
    float res;

    double w=_all.pix.width();
    double h=_all.pix.height();

    double dMi = _all._dispMin;
    double dMa = _all._dispMax;

    res = (gval-dMi)/(dMa-dMi)*w;

    ok= (res>=0 && res<w);

    return res;
}

float xVHistoDlg::mapGValToZoomPixmap(float gval,bool& ok)
{
    float res;

    double w=_zoom.pix.width();
    double h=_zoom.pix.height();

    double dMi = _zoom._dispMin;
    double dMa = _zoom._dispMax;

    res = (gval-dMi)/(dMa-dMi)*w;

    ok= (res>=0 && res<w);

    return res;
}

QPointF xVHistoDlg::mapPxFrmAllPixmapToGValAndIntensity(QPointF px,bool& ok)
{
    QPointF gValAndIntensity;

    double w=_all.pix.width();
    double h=_all.pix.height()-1;

    double dMi = _all._dispMin;
    double dMa = _all._dispMax;

    gValAndIntensity.setX((px.x()/w)*(dMa-dMi)+dMi);
    gValAndIntensity.setY(max(0.0,min(1.0,1.0-px.y()/h)));

    ok = gValAndIntensity.y()>=0 && gValAndIntensity.y()<=1.0 && gValAndIntensity.x()>=_all._dispMin && gValAndIntensity.x()<=_all._dispMax;

    return gValAndIntensity;
}

QPointF xVHistoDlg::mapPxFrmZoomPixmapToGValAndIntensity(QPointF px,bool &ok)
{
    QPointF gValAndIntensity;

    double w=_zoom.pix.width();
    double h=_zoom.pix.height()-1;

    double dMi = _zoom._dispMin;
    double dMa = _zoom._dispMax;

    gValAndIntensity.setX((px.x()/w)*(dMa-dMi)+dMi);
    gValAndIntensity.setY(max(0.0,min(1.0,1.0-px.y()/h)));

    ok = gValAndIntensity.y()>=0 && gValAndIntensity.y()<=1.0 && gValAndIntensity.x()>=_all._dispMin && gValAndIntensity.x()<=_all._dispMax;

    return gValAndIntensity;
}

void xVHistoDlg::dispCursorPos(QPointF p)
{
    bool _zoom = sender()==pZoomHistoPixItem;
    QPointF mappedP;
    bool ok;
    _zoom ? mappedP=mapPxFrmZoomPixmapToGValAndIntensity(p,ok) : mappedP=mapPxFrmAllPixmapToGValAndIntensity(p,ok);
    ok ? pStatusLab->setText(QString("%1").arg(mappedP.x())) : pStatusLab->setText("invalid");
}
