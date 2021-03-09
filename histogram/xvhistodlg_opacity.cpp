#include "xvhistodlg.h"
#include <vtkPiecewiseFunction.h>

void xVHistoDlg::setOpacityFunc(vtkPiecewiseFunctionPtr f)
{
    // copy for cancel
    opacityFuncRef=f;
    cpyOpacityFuncRef=vtkPiecewiseFunction::New();
    cpyOpacityFuncRef->DeepCopy(opacityFuncRef);
}

void xVHistoDlg::createOpacityFunc()
{
    // clear opacity NodeLst
    for (int i=0;i<_opacityNodeLst.count();++i)
    {
        pHistoGV->scene()->removeItem(_opacityNodeLst[i]);
        //delete _opacityNodeLst[i];
    }
    _opacityNodeLst.clear();

    double h=pAllHistoPixItem->pixmap().height();
    // generate opacity node lst
    int i=0;
    bool ok;
    double val[4];
    while (opacityFuncRef->GetNodeValue(i,val)!=-1)
    {
        float pos=mapGValToAllPixmap(val[0],ok);

        xVHistoNodeItem* pItem;
        if (i>0 && i<(opacityFuncRef->GetSize()-1))
            pItem=new xVHistoNodeItem(i,xVHistoNodeItem::HNIM_VERTICAL|xVHistoNodeItem::HNIM_HORIZONTAL|xVHistoNodeItem::HNIM_DELETABLE,pAllHistoPixItem);
        else
            pItem=new xVHistoNodeItem(i,xVHistoNodeItem::HNIM_VERTICAL,pAllHistoPixItem);
        pItem->setLimits(QPointF(0,0),QPointF(pAllHistoPixItem->pixmap().width()-1,pAllHistoPixItem->pixmap().height()-1));
        pItem->setParentItem(pAllHistoPixItem);
        pItem->setColor(QColor::fromRgbF(0,0,0,0));
        pItem->setShapeColor(_paramMp["control node color"]._value.value<QColor>());
        pItem->setHighlightColor(_paramMp["control node color"]._value.value<QColor>());
        pItem->setZValue(5);
        pItem->setPos(pos,h-val[1]*h);
        connect(pItem,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(nodeMoved(QPointF,const int&,xVHistoNodeItem*)));
        connect(pItem,SIGNAL(aboutToBeKilled(xVHistoNodeItem*)),this,SLOT(killNode(xVHistoNodeItem*)));
        _opacityNodeLst.append(pItem);

        ++i;
    }
    pAllHistoPixItem->setHandlesChildEvents(false);
    updateOpacityFunc();
}

void xVHistoDlg::updateOpacityFunc()
{
    if (!opacityFuncRef || _mode!=HDM_OPACITY) return;
    if (!pOpacityPathItem)
    {
        pOpacityPathItem = new QGraphicsPathItem();
        pOpacityPathItem->setZValue(3);
        pOpacityPathItem->setParentItem(pAllHistoPixItem);
    }
    pOpacityPathItem->setPen(QPen(_paramMp["opacity function color"]._value.value<QColor>(),2));

    // update Zoom Path
    if (!pZoomOpacityPathItem)
    {
        pZoomOpacityPathItem = new QGraphicsPathItem();
        pZoomOpacityPathItem->setZValue(3);
        pZoomOpacityPathItem->setParentItem(pZoomHistoPixItem);
        pZoomHistoPixItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape,true);
    }
    pZoomOpacityPathItem->setPen(QPen(_paramMp["opacity function color"]._value.value<QColor>(),2));

    QPainterPath zoomPath;
    QPainterPath path;
    bool ok;
    double h=pAllHistoPixItem->pixmap().height();
    double hz=pZoomHistoPixItem->pixmap().height();
    double val[4];
    opacityFuncRef->GetNodeValue(0,val);
    float pos=mapGValToAllPixmap(val[0],ok);
    float posz=mapGValToZoomPixmap(val[0],ok);

    path.moveTo(pos,h-val[1]*h);
    zoomPath.moveTo(posz,hz-val[1]*hz);
    for (int i=1;i<opacityFuncRef->GetSize();++i)
    {
        opacityFuncRef->GetNodeValue(i,val);
        pos=mapGValToAllPixmap(val[0],ok);
        posz=mapGValToZoomPixmap(val[0],ok);
        path.lineTo(pos,h-val[1]*h);
        zoomPath.lineTo(posz,hz-val[1]*hz);
    }

    pOpacityPathItem->setPath(path);
    pZoomOpacityPathItem->setPath(zoomPath);
}

void xVHistoDlg::nodeMoved(QPointF p,const int& i,xVHistoNodeItem* pNode)
{
    emit KSignal(ST_MSG,new QString("moved"));
    switch (_mode)
    {
    case HDM_OPACITY:
    {
        // calculate val
        double h=pAllHistoPixItem->pixmap().height();
        bool ok;
        double val[4];
        QPointF pnt=mapPxFrmAllPixmapToGValAndIntensity(p,ok);

        if (i>0 && i<opacityFuncRef->GetSize()-1)
        {
            opacityFuncRef->GetNodeValue(i-1,val);
            if (pnt.x()<=val[0]) pnt.setX(val[0]+1);
            opacityFuncRef->GetNodeValue(i+1,val);
            if (pnt.x()>=val[0]) pnt.setX(val[0]-1);
        }
        opacityFuncRef->GetNodeValue(i,val);
        val[0]=pnt.x();
        val[1]=pnt.y();
        opacityFuncRef->SetNodeValue(i,val);

        //move pnt
        for (int j=0;j<_opacityNodeLst.count();++j)
            if (_opacityNodeLst[j]->nr()==i)
                _opacityNodeLst[j]->setPos(QPointF(mapGValToAllPixmap(pnt.x(),ok),h-pnt.y()*h));

        updateOpacityFunc();
        pColorBarPixItem->setPixmap(generateColorBar(pColorBarPixItem->pixmap().width(),pColorBarPixItem->pixmap().height()));

        // trigger update
        if (_paramMp["live mode"]._value.toBool()) emit modified("opacity function");
    }
        break;
    case HDM_LUT:
    {
        // calculate val
        bool ok;
        double val[6];
        double h=pColorBarPixItem->pixmap().height();
        bool _zoomNode = pNode->nr()>=1000;
        QPointF pnt;
        long j=i;
        if (_zoomNode)
        {
            j-=1000;
            pnt=mapPxFrmZoomPixmapToGValAndIntensity(p,ok);
        }
        else
            pnt=mapPxFrmAllPixmapToGValAndIntensity(p,ok);

        if (j>0 && j<colorTransFuncRef->GetSize()-1)
        {
            colorTransFuncRef->GetNodeValue(j-1,val);
            if (pnt.x()<=val[0]) pnt.setX(val[0]+1);
            colorTransFuncRef->GetNodeValue(j+1,val);
            if (pnt.x()>=val[0]) pnt.setX(val[0]-1);
        }
        colorTransFuncRef->GetNodeValue(j,val);
        val[0]=pnt.x();
        val[1]=pNode->color().redF();
        val[2]=pNode->color().greenF();
        val[3]=pNode->color().blueF();
        colorTransFuncRef->SetNodeValue(j,val);

        if (_paramMp["histogram colorization"]._value.toBool() && _mode==HDM_LUT)
        {
            _all=generatePixmap(_all._dispMin,_all._dispMax,pAllHistoPixItem->pixmap().width(),pAllHistoPixItem->pixmap().height());
            pAllHistoPixItem->setPixmap(_all.pix);
            _zoom=generatePixmap(_zoom._dispMin,_zoom._dispMax,pZoomHistoPixItem->pixmap().width(),pZoomHistoPixItem->pixmap().height());
            pZoomHistoPixItem->setPixmap(_zoom.pix);
        }
        updateColorFunc();

        // trigger update
        if (_paramMp["live mode"]._value.toBool()) emit modified("color function");
    }
        break;
    }

}

