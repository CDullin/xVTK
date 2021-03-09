#include "xvhistodlg.h"
#include <vtkColorTransferFunction.h>

void xVHistoDlg::setColorTransFunc(vtkColorTransferFunctionPtr f)
{
    // copy for cancel
    colorTransFuncRef=f;
    cpyColorTransFuncRef=vtkColorTransferFunction::New();
    cpyColorTransFuncRef->DeepCopy(colorTransFuncRef);
}

void xVHistoDlg::updateColorFunc()
{
    if (!colorTransFuncRef || _mode!=HDM_LUT) return;
    double h=pZoomColorBarPixItem->pixmap().height();

    for (int i=0;i<_colorZoomNodeLst.count();++i)
    {
        bool ok;
        double val[6];
        colorTransFuncRef->GetNodeValue(i,val);
        float zpos=mapGValToZoomPixmap(val[0],ok);
        xVHistoNodeItem* pZItem=_colorZoomNodeLst[i];
        pZItem->setPos(zpos,h/2);
        pZItem->setVisible(zpos>=0 && zpos<=pZoomColorBarPixItem->pixmap().width());
    }
    for (int i=0;i<_colorNodeLst.count();++i)
    {
        bool ok;
        double val[6];
        colorTransFuncRef->GetNodeValue(i,val);
        float zpos=mapGValToAllPixmap(val[0],ok);
        xVHistoNodeItem* pZItem=_colorNodeLst[i];
        pZItem->setPos(zpos,h/2);
    }
    pZoomColorBarPixItem->setPixmap(generateColorBar(pZoomColorBarPixItem->pixmap().width(),h,true));
    pColorBarPixItem->setPixmap(generateColorBar(pColorBarPixItem->pixmap().width(),pColorBarPixItem->pixmap().height()));
}

void xVHistoDlg::createColorFunc()
{
    if (!colorTransFuncRef) return;
    // clear opacity NodeLst
    for (int i=0;i<_colorNodeLst.count();++i)
    {
        pHistoGV->scene()->removeItem(_colorNodeLst[i]);
        //delete _opacityNodeLst[i];
    }
    _colorNodeLst.clear();

    // clear zoom node list
    for (int i=0;i<_colorZoomNodeLst.count();++i)
        pHistoGV->scene()->removeItem(_colorZoomNodeLst[i]);
    _colorZoomNodeLst.clear();

    double h=pColorBarPixItem->pixmap().height();
    // generate opacity node lst
    int i=0;
    bool ok;
    double val[6];
    while (colorTransFuncRef->GetNodeValue(i,val)!=-1)
    {
        float pos=mapGValToAllPixmap(val[0],ok);
        float zpos=mapGValToZoomPixmap(val[0],ok);
        xVHistoNodeItem* pItem,*pZItem;

        if (i>0 && i<(colorTransFuncRef->GetSize()-1))
        {
            pItem=new xVHistoNodeItem(i,xVHistoNodeItem::HNIM_HORIZONTAL|xVHistoNodeItem::HNIM_DELETABLE|xVHistoNodeItem::HNIM_COLOR,pAllHistoPixItem);
            pZItem=new xVHistoNodeItem(i+1000,xVHistoNodeItem::HNIM_HORIZONTAL|xVHistoNodeItem::HNIM_DELETABLE|xVHistoNodeItem::HNIM_COLOR,pZoomHistoPixItem);
        }
        else
        {
            pItem=new xVHistoNodeItem(i,xVHistoNodeItem::HNIM_FIXED|xVHistoNodeItem::HNIM_COLOR,pAllHistoPixItem);
            pZItem=new xVHistoNodeItem(i+1000,xVHistoNodeItem::HNIM_FIXED|xVHistoNodeItem::HNIM_COLOR,pZoomHistoPixItem);
        }

        pItem->setLimits(QPointF(0,0),QPointF(pColorBarPixItem->pixmap().width()-1,pColorBarPixItem->pixmap().height()-1));
        pItem->setParentItem(pColorBarPixItem);
        pItem->setColor(QColor::fromRgbF(val[1],val[2],val[3]));
        pItem->setShapeColor(_paramMp["control node color"]._value.value<QColor>());
        pItem->setHighlightColor(_paramMp["control node color"]._value.value<QColor>());
        pItem->setZValue(5);
        pItem->setPos(pos,h/2);        
        connect(pItem,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(nodeMoved(QPointF,const int&,xVHistoNodeItem*)));
        connect(pItem,SIGNAL(colorChanged(xVHistoNodeItem*)),this,SLOT(nodeColorChanged(xVHistoNodeItem*)));
        connect(pItem,SIGNAL(aboutToBeKilled(xVHistoNodeItem*)),this,SLOT(killNode(xVHistoNodeItem*)));
        _colorNodeLst.append(pItem);

        pZItem->setLimits(QPointF(0,0),QPointF(pZoomColorBarPixItem->pixmap().width()-1,pZoomColorBarPixItem->pixmap().height()-1));
        pZItem->setParentItem(pZoomColorBarPixItem);
        pZItem->setColor(QColor::fromRgbF(val[1],val[2],val[3]));
        pZItem->setShapeColor(_paramMp["control node color"]._value.value<QColor>());
        pZItem->setHighlightColor(_paramMp["control node color"]._value.value<QColor>());
        pZItem->setZValue(5);
        pZItem->setPos(zpos,h/2);
        connect(pZItem,SIGNAL(moved(QPointF,const int&,xVHistoNodeItem*)),this,SLOT(nodeMoved(QPointF,const int&,xVHistoNodeItem*)));
        connect(pZItem,SIGNAL(colorChanged(xVHistoNodeItem*)),this,SLOT(nodeColorChanged(xVHistoNodeItem*)));
        connect(pZItem,SIGNAL(aboutToBeKilled(xVHistoNodeItem*)),this,SLOT(killNode(xVHistoNodeItem*)));
        _colorZoomNodeLst.append(pZItem);
        pZItem->setVisible(zpos>=0 && zpos<=pZoomColorBarPixItem->pixmap().width());

        ++i;
    }
    pColorBarPixItem->setHandlesChildEvents(false);
}

void xVHistoDlg::nodeColorChanged(xVHistoNodeItem* pNode)
{
    if (!pNode) return;
    bool ok;
    bool _zoomNode = pNode->nr()>=1000;
    long j=pNode->nr();
    QPointF pnt;
    if (_zoomNode)
    {
        j-=1000;
        pnt=mapPxFrmZoomPixmapToGValAndIntensity(pNode->pos(),ok);
    }
    else
        mapPxFrmAllPixmapToGValAndIntensity(pNode->pos(),ok);
    double val[6];
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

void xVHistoDlg::dblClickInColorBar(QPointF p)
{
    if (_mode!=HDM_LUT || !colorTransFuncRef) return;
    bool ok;
    QPointF val=mapPxFrmAllPixmapToGValAndIntensity(p,ok);
    double col[3];
    colorTransFuncRef->GetColor(val.x(),col);
    colorTransFuncRef->AddRGBPoint(val.x(),col[0],col[1],col[2]);

    if (_paramMp["histogram colorization"]._value.toBool() && _mode==HDM_LUT)
    {
        _all=generatePixmap(_all._dispMin,_all._dispMax,pAllHistoPixItem->pixmap().width(),pAllHistoPixItem->pixmap().height());
        pAllHistoPixItem->setPixmap(_all.pix);
        _zoom=generatePixmap(_zoom._dispMin,_zoom._dispMax,pZoomHistoPixItem->pixmap().width(),pZoomHistoPixItem->pixmap().height());
        pZoomHistoPixItem->setPixmap(_zoom.pix);
    }
    createColorFunc();
}

void xVHistoDlg::dblClickInZoomColorBar(QPointF p)
{
    if (_mode!=HDM_LUT || !colorTransFuncRef) return;
    bool ok;
    QPointF val=mapPxFrmZoomPixmapToGValAndIntensity(p,ok);
    double col[3];
    colorTransFuncRef->GetColor(val.x(),col);
    colorTransFuncRef->AddRGBPoint(val.x(),col[0],col[1],col[2]);

    if (_paramMp["histogram colorization"]._value.toBool() && _mode==HDM_LUT)
    {
        _all=generatePixmap(_all._dispMin,_all._dispMax,pAllHistoPixItem->pixmap().width(),pAllHistoPixItem->pixmap().height());
        pAllHistoPixItem->setPixmap(_all.pix);
        _zoom=generatePixmap(_zoom._dispMin,_zoom._dispMax,pZoomHistoPixItem->pixmap().width(),pZoomHistoPixItem->pixmap().height());
        pZoomHistoPixItem->setPixmap(_zoom.pix);
    }
    createColorFunc();
}
