#include "xvhistodlg.h"
#include <math.h>
#include <stdlib.h>

using namespace std;

QPixmap xVHistoDlg::bar(const int& mode,const int& w,const int& h,const double& miV,const double& maV,void* data)
{
    QPixmap pix(w,h);
    pix.fill(QColor(Qt::black));
    QPainter pain(&pix);
    QBrush br(QPixmap("://images/opacity_pattern.png"));
    pain.setPen(Qt::white);
    pain.setBrush(br);
    pain.drawRect(QRectF(0,0,w,h));

    switch (mode)
    {
    case HDM_LUT:
    {
        vtkColorTransferFunctionPtr colorTransFuncRef=(vtkColorTransferFunctionPtr)data;
        QPen pen(Qt::blue);
        double col[3];
        for (int i=0;i<w;++i)
        {
            double val = (double)i/w*(maV-miV)+miV;
            colorTransFuncRef->GetColor(val,col);
            pen.setColor(QColor::fromRgbF(col[0],col[1],col[2]));
            pain.setPen(pen);
            pain.drawLine(i,0,i,h-1);
        }
    }
        break;
    case HDM_OPACITY:
    {
        vtkPiecewiseFunctionPtr opacityFuncRef=(vtkPiecewiseFunctionPtr)data;
        QPen pen(Qt::blue);
        for (int i=0;i<w;++i)
        {
            double val = (double)i/w*(maV-miV)+miV;
            pen.setColor(QColor(0,0,255,255.0*opacityFuncRef->GetValue(val)));
            pain.setPen(pen);
            pain.drawLine(i,0,i,h-1);
        }
    }
        break;
    case HDM_MEASURE:
        break;
    case HDM_WINDOW_LEVEL:
        break;
    }

    pain.setPen(Qt::white);
    pain.setBrush(Qt::NoBrush);
    pain.drawRect(QRectF(0,0,w-1,h-1));
    pain.end();
    return pix;
}
QPixmap xVHistoDlg::generateColorBar(int w,int h,bool zoom)
{
    QPixmap pix(w,h);
    pix.fill(QColor(Qt::black));
    QPainter pain(&pix);
    QBrush br(QPixmap("://images/opacity_pattern.png"));
    pain.setPen(Qt::white);
    pain.setBrush(br);
    pain.drawRect(QRectF(0,0,w,h));

    switch (_mode)
    {
    case HDM_LUT:
    {
        if (colorTransFuncRef)
        {
            QPen pen(Qt::blue);
            double col[3];
            bool ok;
            for (int i=0;i<w;++i)
            {
                if (zoom)
                {
                    QPointF mappedP=mapPxFrmZoomPixmapToGValAndIntensity(QPointF(i,0),ok);
                    if (ok)
                    {
                        colorTransFuncRef->GetColor(mappedP.x(),col);
                        pen.setColor(QColor::fromRgbF(col[0],col[1],col[2]));
                    }
                    else
                        pen.setColor(QColor(0,0,0,0));
                }
                else
                {
                    QPointF mappedP=mapPxFrmAllPixmapToGValAndIntensity(QPointF(i,0),ok);
                    if (ok)
                    {
                        colorTransFuncRef->GetColor(mappedP.x(),col);
                        pen.setColor(QColor::fromRgbF(col[0],col[1],col[2]));
                    }
                    else
                        pen.setColor(QColor(0,0,0,0));
                }
                pain.setPen(pen);
                pain.drawLine(i,0,i,h-1);
            }
        }
    }
        break;
    case HDM_OPACITY:
    {
        if (opacityFuncRef)
        {
            QPen pen(Qt::blue);
            bool ok;
            for (int i=0;i<w;++i)
            {
                if (zoom)
                {
                    QPointF mappedP=mapPxFrmZoomPixmapToGValAndIntensity(QPointF(i,0),ok);
                    if (ok)
                        pen.setColor(QColor(0,0,255,255.0*opacityFuncRef->GetValue(mappedP.x())));
                    else
                        pen.setColor(Qt::red);
                }
                else {
                    QPointF mappedP=mapPxFrmAllPixmapToGValAndIntensity(QPointF(i,0),ok);
                    if (ok)
                        pen.setColor(QColor(0,0,255,255.0*opacityFuncRef->GetValue(mappedP.x())));
                    else
                        pen.setColor(Qt::red);
                }
                pain.setPen(pen);
                pain.drawLine(i,0,i,h-1);
            }
        }
    }
        break;
    case HDM_MEASURE:
        break;
    case HDM_WINDOW_LEVEL:
        break;
    }

    pain.setPen(Qt::white);
    pain.setBrush(Qt::NoBrush);
    pain.drawRect(QRectF(0,0,w-1,h-1));
    pain.end();
    return pix;
}

xVHISTO_PIXMAP xVHistoDlg::generatePixmap(const float& _dMin,const float& _dMax,const int& w,const int& h)
{
    xVHISTO_PIXMAP res;
    res.pix = QPixmap(w,h);
    res.pix.fill(_paramMp["background color"]._value.value<QColor>());
    res._dispMax=_dMax;
    res._dispMin=_dMin;

    if (pData)
    {

        double mi = pData->info()._dataTypeMin;
        double ma = pData->info()._dataTypeMax;

        // calculate display version of the histogram
        double *dispHisto=(double*)malloc(w*sizeof(double));
        memset(dispHisto,0,w*sizeof(double));
        double maxBin=0;
        double swx,ewx,rel,sproc,eproc;

        for (int x=0;x<w-1;++x)
        {
            rel = double(x)/double(w);
            rel = rel*(_dMax-_dMin)+_dMin;
            rel = (rel-mi)/(ma-mi)*(float)(pData->info()._binCount-1);
            swx = rel;
            rel = double(x+1)/double(w);
            rel = rel*(_dMax-_dMin)+_dMin;
            rel = (rel-mi)/(ma-mi)*(float)(pData->info()._binCount-1);
            ewx = max(swx+1,rel);

            // 1-proc at starting point, proc at end point, inbetween 1
            sproc=(1.0-swx/floor(swx));
            eproc=ewx/floor(ewx);
            dispHisto[x]+=sproc*pData->info()._bins[(int)floor(swx)];
            dispHisto[x]+=eproc*pData->info()._bins[(int)ceil(ewx)];

            double weightSum=sproc+eproc;

            for (long j=floor(swx)+1;j<ceil(ewx);++j)
            {
                dispHisto[x]+=pData->info()._bins[j];
                ++weightSum;
            }
            weightSum>0 ? dispHisto[x]/=weightSum : dispHisto[x]=0;
            if (_paramMp["histogram mode"]._value.toString()=="logarithmic")
            {
                dispHisto[x]>0 ? dispHisto[x]=log(dispHisto[x])/log(10.0) : dispHisto[x]=0;
            }
            maxBin=max(maxBin,dispHisto[x]);
        }

        //logarithmic
        QPen p(_paramMp["histogram color"]._value.value<QColor>());
        QPainter pain(&res.pix);
        pain.setPen(p);
        double val,col[3];
        for (int x=0;x<w;++x)
            if (dispHisto[x]>0)
            {
                if (_paramMp["histogram colorization"]._value.toBool() && _mode==HDM_LUT)
                {
                    rel = double(x)/double(w);
                    rel = rel*(_dMax-_dMin)+_dMin;
                    swx = rel;
                    rel = double(x+1)/double(w);
                    rel = rel*(_dMax-_dMin)+_dMin;
                    ewx = max(swx+1,rel);
                    val=(swx+ewx)/2.0;
                    colorTransFuncRef->GetColor(val,col);
                    pain.setPen(QColor::fromRgbF(col[0],col[1],col[2]));
                }
                pain.drawLine(x,h-1,x,h-dispHisto[x]/maxBin*h);
            }
        pain.end();

        res._valMin=0;
        res._valMax=maxBin;
    }
    return res;
}
