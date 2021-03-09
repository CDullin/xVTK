#include "xVHisto.h"

#include <QtConcurrent>

#include <stdlib.h>
#include <math.h>

using namespace std;

xVHisto::xVHisto(xVVolObj* pObj):QObject(pObj)
{
    pDataRef = pObj;
    update();
}

HISTO_DATA* generateHistoDataForSlices(HISTO_DATA* pHData)
{
    double val;
    double ma,mi,br;
    ma = pHData->pData->GetScalarTypeMax();
    mi = pHData->pData->GetScalarTypeMin();
    br = pHData->_binCount-1;
    for (long z=pHData->_zmin;z<pHData->_zmax;++z)
        for (long y=0;y<pHData->_ymax;++y)
            for (long x=0;x<pHData->_xmax;++x)
            {
               // only 1 component in the moment
               val = pHData->pData->GetScalarComponentAsDouble(x,y,z,0);
               pHData->_quadSumBuffer+=pow(val,2);
               pHData->_sumBuffer+=val;
               pHData->_minVal=min(pHData->_minVal,val);
               pHData->_maxVal=max(pHData->_maxVal,val);
               pHData->_bins[(int)((val-mi)/(ma-mi)*br)]++;
            }
    pHData->_count = pHData->_ymax*pHData->_xmax*(pHData->_zmax-pHData->_zmin);
    return pHData;
}

void xVHisto::setValue(int v)
{
    emit KSignal(ST_SET_PROCESS,new int(v));
}

void xVHisto::setRange(int l,int h)
{
    emit KSignal(ST_SET_PROCESS_RANGE,new QPoint(l,h));
}

void xVHisto::update()
{
    if (pDataRef && pDataRef->data() && _info._status==VOL_DATA_INFO::VDTS_UPDATE_NEEDED)
    {
        // find out data type, assign bin count and mapping function
        switch (pDataRef->data()->GetScalarType())
        {
        case VTK_CHAR:              _info._binCount=256.0;   break;
        case VTK_UNSIGNED_CHAR:     _info._binCount=256.0;   break;
        case VTK_SHORT:             _info._binCount=2048.0;  break;
        case VTK_UNSIGNED_SHORT:    _info._binCount=2048.0;  break;
        case VTK_INT:               _info._binCount=2048.0;  break;
        case VTK_UNSIGNED_INT:      _info._binCount=2048.0;  break;
        case VTK_FLOAT:             _info._binCount=2048.0;  break;
        case VTK_DOUBLE:            _info._binCount=2048.0;  break;
        }

        if (_info._bins) free(_info._bins);
        _info._bins=(double*)malloc(_info._binCount*sizeof(double));
        memset(_info._bins,0,_info._binCount*sizeof(double));

        int dim[3];
        pDataRef->data()->GetDimensions(dim);

        _info._dataTypeMax = pDataRef->data()->GetScalarTypeMax();
        _info._dataTypeMin = pDataRef->data()->GetScalarTypeMin();

        emit KSignal(ST_SET_PROCESS_TXT,new QString("updating histogram"));

        int threadCount=QThread::idealThreadCount()*ceil(max(1.0f,dim[2]/20.0f));
        int zInc=dim[2]/threadCount;
        QList <HISTO_DATA*> zStack;
        int swz=0;
        do
        {
            HISTO_DATA *pTmpHisto=new HISTO_DATA();
            pTmpHisto->_binCount=_info._binCount;
            pTmpHisto->_count=0.0;
            pTmpHisto->_sumBuffer=0.0f;
            pTmpHisto->_quadSumBuffer=0.0f;
            pTmpHisto->_bins=(double*)realloc(pTmpHisto->_bins,pTmpHisto->_binCount*sizeof(double));
            memset(pTmpHisto->_bins,0,pTmpHisto->_binCount*sizeof(double));
            pTmpHisto->_xmax = dim[0];
            pTmpHisto->_ymax = dim[1];
            pTmpHisto->_zmin = swz;
            pTmpHisto->_zmax = min(swz+zInc,dim[2]-1)+1;
            pTmpHisto->pData = pDataRef->data();

            zStack.append(pTmpHisto);
            swz=min(swz+zInc+1,dim[2]-1);
        }
        while ((swz+1)<dim[2]);

        QFutureWatcher <HISTO_DATA*> futureWatcher;
        // setup progress callbacks
        QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), this, SLOT(setRange(int,int)));
        QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), this, SLOT(setValue(int)));
        // start and wait
        futureWatcher.setFuture(QtConcurrent::mapped(zStack,generateHistoDataForSlices));
        do
        {
            QCoreApplication::processEvents();
        }
        while(futureWatcher.isRunning());

        // sum up all results
        for (QList <HISTO_DATA*>::iterator it=zStack.begin();it<zStack.end();++it)
        {
            // sum values
            if ((*it)->_count>0)
            {
                _info._count+=(*it)->_count;
                _info._sumBuffer+=(*it)->_sumBuffer;
                _info._quadSumBuffer+=(*it)->_quadSumBuffer;
                _info._minVal=min(_info._minVal,(*it)->_minVal);
                _info._maxVal=max(_info._maxVal,(*it)->_maxVal);
                // copy histo values
                for (int i=0;i<(*it)->_binCount;++i)
                    _info._bins[i]+=(*it)->_bins[i];
            }
            // destroy buffer
            //if ((*it)->_bins) free((*it)->_bins);(*it)->_bins=NULL;
        }

        // calculate values
        if (_info._count>0)
        {
            _info._average=(_info._sumBuffer)/_info._count;
            _info._std=sqrt((_info._quadSumBuffer)/_info._count-pow(_info._average,2));
            // calculate quantils
            double sumBuf=0;
            _info._maxBinValue=0;
            for (int b=0;b<_info._binCount;++b)
            {
                sumBuf+=_info._bins[b];
                _info._maxBinValue=max(_info._maxBinValue,_info._bins[b]);
            }
            double facPattern[5]={0.05,0.25,0.5,0.75,0.95};
            double sumUp=0.0;
            for (int j=0;j<5;++j)
                facPattern[j]*=sumBuf;
            int qNr=0;
            for (int b=0;b<_info._binCount;++b)
            {
                sumUp+=_info._bins[b];
                if (sumUp>facPattern[qNr] && qNr<5)
                {
                    _info._q[qNr++]=b;
                }
            }
        }

        emit KSignal(ST_HISTO_UPDATED,pDataRef);
        _info._status = VOL_DATA_INFO::VDTS_VALID;
    }
}
