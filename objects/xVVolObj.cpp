#include "xVVolObj.h"

#include <QVector3D>
#include <vtkImageReader.h>
#include "xVTypes.h"
#include <qmetatype.h>
#include "vtkImageData.h"

xVVolObj::xVVolObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_VOLUME;
    _paramMp["volume file type"]._id=1;
    _paramMp["volume file type"]._value="raw";
    _paramMp["file name"]._id=2;
    _paramMp["file name"]._value=QVariant::fromValue(QFileInfo(::_settings["last volume file"]._value.toString()));
    _paramMp["dimension"]._id=3;
    _paramMp["dimension"]._value=QVariant::fromValue(x3D_SAMPLE_POS(512,512,512));
    _paramMp["dimension"]._subGrp="raw";
    _paramMp["resolution"]._id=4;
    _paramMp["resolution"]._value=QVariant(QVector3D(0.04,0.04,0.04));
    _paramMp["resolution"]._subGrp="raw";
    _paramMp["position"]._id=5;
    _paramMp["position"]._value=QVariant(QVector3D(0,0,0));
    _paramMp["position"]._subGrp="raw";
    _paramMp["data type"]._id=6;
    _paramMp["data type"]._value="uint16";
    _paramMp["data type"]._subGrp="raw";
    _paramMp["channel count"]._id=7;
    _paramMp["channel count"]._value=1;
    _paramMp["channel count"]._subGrp="raw";
    _paramMp["header size"]._id=8;
    _paramMp["header size"]._value=0;
    _paramMp["header size"]._subGrp="raw";
    _paramMp["gap size"]._id=9;
    _paramMp["gap size"]._value=0;
    _paramMp["gap size"]._subGrp="raw";
    _paramMp["little endian"]._id=10;
    _paramMp["little endian"]._value=true;
    _paramMp["little endian"]._subGrp="raw";

    pProgObserver = new xVProgressObserver(this);
    pHisto = new xVHisto(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
    connect(pHisto,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
}

xVVolObj::xVVolObj(QDataStream &d):xVGenImpObj(d)
{
    _type = xVOT_VOLUME;

    pProgObserver = new xVProgressObserver(this);
    pHisto = new xVHisto(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
    connect(pHisto,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
}

void xVVolObj::reset()
{
    if (_status!=OS_UPDATE_NEEDED)
    {
        // free all resources
        if (pDataImporter)
        {
            pDataImporter->RemoveAllInputs();
            pDataImporter->Delete();
        }
        pDataImporter=nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
}

void xVVolObj::run()
{
    // check if we get data streamed or if we load it
    QFileInfo pInfo=_paramMp["file name"]._value.value<QFileInfo>();
    if (pInfo.isFile() && pInfo.exists())
    {
        setStatus(OS_RUNNING);
        const char* pattern[]={"vox","vff","vtk","vol","raw"};
        int fileType=-1;
        for (int i=0;i<5;++i)
            if (_paramMp["volume file type"]._value.toString()==pattern[i])
                fileType=i;
        switch (fileType)
        {
        case 0 : // vox            
            break;
        case 1 : // vff
            break;
        case 2 : // vtk
            break;
        case 3 : // vol
            break;
        case 4 : // raw
        {
            pDataImporter=vtkImageReader::New();
            pDataImporter->SetFileName(pInfo.absoluteFilePath().toLocal8Bit().constData());
            pDataImporter->SetFileDimensionality(3);
            switch (::_optionLsts["data type"].indexOf(_paramMp["data type"]._value.toString()))
            {
            case 0 : // int8
                pDataImporter->SetDataScalarTypeToChar();
                break;
            case 1 : // uint8
                pDataImporter->SetDataScalarTypeToUnsignedChar();
                break;
            case 2 : // int16
                pDataImporter->SetDataScalarTypeToShort();
                break;
            case 3 : // uint16
                pDataImporter->SetDataScalarTypeToUnsignedShort();
                break;
            case 4 : // int32
                pDataImporter->SetDataScalarTypeToInt();
                break;
            case 5 : // uint32
                pDataImporter->SetDataScalarTypeToUnsignedInt();
                break;
            case 6 : // float
                pDataImporter->SetDataScalarTypeToFloat();
                break;
            case 7 : // double
                pDataImporter->SetDataScalarTypeToDouble();
                break;
            }
            pDataImporter->SetNumberOfScalarComponents(_paramMp["channel count"]._value.toInt());
            x3D_SAMPLE_POS dim = _paramMp["dimension"]._value.value<x3D_SAMPLE_POS>();
            pDataImporter->SetDataExtent(0,dim.x-1,0,dim.y-1,0,dim.z-1);
            QVector3D res=_paramMp["resolution"]._value.value<QVector3D>();
            //pDataImporter->SetDataSpacing(res.x(),res.y(),res.z());
            pDataImporter->SetDataSpacing(1,1,1);
            pDataImporter->SetHeaderSize(_paramMp["header size"]._value.toLongLong());
            emit KSignal(ST_SET_PROCESS_RANGE,
                         new QPoint(
                             pDataImporter->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                             pDataImporter->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
            emit KSignal(ST_SET_PROCESS_TXT,new QString("loading ... "));

            pDataImporter->SetProgressObserver(pProgObserver);
            QVector3D pos=_paramMp["position"]._value.value<QVector3D>();
            pDataImporter->SetDataOrigin(pos.x(),pos.y(),pos.z());
            pDataImporter->Update();

            pDataPtr = pDataImporter->GetOutput();
            if (pHisto) pHisto->update();

            _outputParamMp["volume data"]._value = QVariant::fromValue(pDataImporter->GetOutput());
            _outputParamMp["volume data"]._id=0;

            setStatus(OS_VALID);
        }
            break;
        default:
            setStatus(OS_ERROR);
            emit KSignal(ST_ERROR_MSG, new QString("unknown file type"));
            break;
        }               
    }
    else
    {
        emit KSignal(ST_ERROR_MSG,new QString("file not found"));
        setStatus(OS_ERROR);
    }

}

void xVVolObj::save(QDataStream &d, bool _explicit)
{
    QFileInfo pInfo=_paramMp["file name"]._value.value<QFileInfo>();
    if (_explicit && pInfo.isFile() && pInfo.exists())
    {
        QFile::copy(pInfo.absoluteFilePath(),_id+"."+pInfo.suffix());
        pInfo.setFile(_id+"."+pInfo.suffix());
        _paramMp["file name"]._value=QVariant::fromValue(pInfo);
    }

    xVGenImpObj::save(d,_explicit);
}
