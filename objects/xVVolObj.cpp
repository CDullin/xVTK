#include "xVVolObj.h"

#include <QVector3D>
#include <vtkImageReader.h>
#include "xVTypes.h"
#include <qmetatype.h>
#include "vtkImageData.h"
#include "xVIOVOX.h"
#include "xVIOVFF.h"

xVVolObj::xVVolObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_VOLUME;
    _paramMp["volume file type"]._id=1;
    _paramMp["volume file type"]._value="raw";
    _paramMp["file name"]._id=2;
    _paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,::_settings["last volume file"]._value.toString()));
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
    xVGenImpObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;
    // check if we get data streamed or if we load it
    QFileInfo pInfo(_paramMp["file name"]._value.value<xFileName>()._fileName);
    if (pInfo.isFile() && pInfo.exists())
    {
        setStatus(OS_RUNNING);
        QStringList fileType;fileType << "VOX" << "VFF" << "VTK" << "VOL" << "RAW";
        switch (fileType.indexOf(pInfo.suffix().toUpper()))
        {
        case 0 : // vox
            {
            xVIOVOX voxImporter(this);
            connect(&voxImporter,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
            pDataPtr = voxImporter.read(pInfo.absoluteFilePath().toLatin1());
            if (pHisto) pHisto->update();
            _outputParamMp["volume data"]._value = QVariant::fromValue(pDataPtr);
            _outputParamMp["volume data"]._id=0;
            _outputParamMp["dimension"]._value=_paramMp["dimension"]._value=
                    QVariant::fromValue(x3D_SAMPLE_POS(voxImporter.info()._dim[0],voxImporter.info()._dim[1],voxImporter.info()._dim[2]));
            _outputParamMp["dimension"]._id=1;
            _outputParamMp["resolution"]._id=2;
            _outputParamMp["resolution"]._value=
                    //QVariant::fromValue(QVector3D(voxImporter.info()._vs[0],voxImporter.info()._vs[1],voxImporter.info()._vs[2]));
                    QVariant::fromValue(QVector3D(1,1,1));
            voxImporter.valid() ? setStatus(OS_VALID) : setStatus(OS_ERROR);
            _paramMp["dimension"]._value=QVariant::fromValue(
                        x3D_SAMPLE_POS(voxImporter.info()._dim[0],voxImporter.info()._dim[1],voxImporter.info()._dim[2]));
            _paramMp["resolution"]._value=QVariant::fromValue(
                        QVector3D(voxImporter.info()._vs[0],voxImporter.info()._vs[1],voxImporter.info()._vs[2]));
            _paramMp["channel count"]._value=1;
            }
            break;
        case 1 : // vff
            {
            xVIOVFF vffImporter(this);
            connect(&vffImporter,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
            pDataPtr = vffImporter.read(pInfo.absoluteFilePath().toLatin1());
            if (pHisto) pHisto->update();
            _outputParamMp["volume data"]._value = QVariant::fromValue(pDataPtr);
            _outputParamMp["volume data"]._id=0;
            _outputParamMp["dimension"]._value=_paramMp["dimension"]._value=
                    QVariant::fromValue(x3D_SAMPLE_POS(vffImporter.info()._dim[0],vffImporter.info()._dim[1],vffImporter.info()._dim[2]));
            _outputParamMp["dimension"]._id=1;
            _outputParamMp["resolution"]._id=2;
            _outputParamMp["resolution"]._value=
                    //QVariant::fromValue(QVector3D(voxImporter.info()._vs[0],voxImporter.info()._vs[1],voxImporter.info()._vs[2]));
                    QVariant::fromValue(QVector3D(1,1,1));
            vffImporter.valid() ? setStatus(OS_VALID) : setStatus(OS_ERROR);
            _paramMp["dimension"]._value=QVariant::fromValue(
                        x3D_SAMPLE_POS(vffImporter.info()._dim[0],vffImporter.info()._dim[1],vffImporter.info()._dim[2]));
            _paramMp["resolution"]._value=QVariant::fromValue(
                        QVector3D(vffImporter.info()._vs[0],vffImporter.info()._vs[1],vffImporter.info()._vs[2]));
            _paramMp["channel count"]._value=1;
            }
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
            pDataImporter->SetDataSpacing(1,1,1);
            pDataImporter->SetHeaderSize(_paramMp["header size"]._value.toLongLong());
            emit KSignal(ST_SET_PROCESS_RANGE,
                         new QPoint(
                             pDataImporter->GetProgressMinValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value,
                             pDataImporter->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value));
            emit KSignal(ST_SET_PROCESS_TXT,new QString("loading ... "));

            pDataImporter->SetProgressObserver(pProgObserver);
            QVector3D pos=_paramMp["position"]._value.value<QVector3D>();
            pDataImporter->SetDataOrigin(pos.x(),pos.y(),pos.z());
            pDataImporter->Update();

            pDataPtr = pDataImporter->GetOutput();
            pDataPtr->SetSpacing(res[0],res[1],res[2]);
            if (pHisto) pHisto->update();

            _outputParamMp["volume data"]._value = QVariant::fromValue(pDataImporter->GetOutput());
            _outputParamMp["volume data"]._id=0;
            _outputParamMp["dimension"]._value=_paramMp["dimension"]._value;
            _outputParamMp["dimension"]._id=1;
            _outputParamMp["resolution"]._id=2;
            _outputParamMp["resolution"]._value=_paramMp["resolution"]._value;

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
