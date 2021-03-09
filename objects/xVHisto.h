#ifndef XVHISTO_H
#define XVHISTO_H

#include "xVTypes.h"
#include "xVVolObj.h"
#include <vtkImageData.h>

class xVVolObj;

struct HISTO_DATA
{
    long double _count=0;
    long double _sumBuffer=0;
    long double _quadSumBuffer=0;
    double _binCount;
    double* _bins=nullptr;
    long _xmax,_ymax,_zmin,_zmax;
    vtkImageDataPtr pData;
    double _minVal=9E12;
    double _maxVal=-9E12;
};


struct VOL_DATA_INFO
{
public:
    enum VOL_DATA_INFO_STATUS
    {
        VDTS_VALID,
        VDTS_UPDATE_NEEDED
    };
    long double _count=0;
    long double _sumBuffer=0;
    long double _quadSumBuffer=0;
    double _minVal=9E12;
    double _maxVal=-9E12;
    double _average=0.0;
    double _std=0.0;
    double _q[5]={0.0,0.0,0.0,0.0,0.0};
    VOL_DATA_INFO_STATUS _status=VDTS_UPDATE_NEEDED;
    double* _bins=nullptr;
    double  _binCount=0xFFFF;
    double  _maxBinValue=0.0;
    double  _dataTypeMin,_dataTypeMax;
};

class xVHisto: public QObject
{
Q_OBJECT
public:
    xVHisto(xVVolObj* parent=nullptr);
    VOL_DATA_INFO info(){return _info;}
    void update();
    void setUpdateNeeded(){_info._status=VOL_DATA_INFO::VDTS_UPDATE_NEEDED;}
signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
protected slots:
    void setRange(int min,int max);
    void setValue(int);
protected:
    xVVolObj *pDataRef=nullptr;
    VOL_DATA_INFO _info;
};

#endif // XVHISTO_H
