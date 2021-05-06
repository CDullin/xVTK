#ifndef XVGENVOLIO_H
#define XVGENVOLIO_H

#include "xVTypes.h"
#include <QObject>

class xVGenVolIO:public QObject
{          
    Q_OBJECT
public:
    struct VOL_FILE_INFO
    {
        bool _littleEndian = true;
        long _dim[3];
        long _pos[3]={0,0,0};
        float _vs[3]={1,1,1};
        long _headerSkip=0;
        long _FrameOffset=0;
        long _dataType;             // index in the datatype stringlist
        int _bytesPerVoxel=1;
    };

    xVGenVolIO(QObject* parent=nullptr);
    virtual vtkImageDataPtr read(const QString& fname)=0;
    virtual void write(vtkImageDataPtr data,const QString& fname)=0;
    VOL_FILE_INFO info(){return _info;}
    bool valid(){return _noError;}

signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);

protected:
    xVProgressObserver* pProgObserver = nullptr;
    VOL_FILE_INFO _info;
    bool _noError = true;
};

#endif // XVGENVOLIO_H
