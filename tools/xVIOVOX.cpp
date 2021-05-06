#include "xVIOVOX.h"
#include <QTextStream>

vtkImageDataPtr xVIOVOX::read(const QString& fname)
{
    vtkImageDataPtr pResult = nullptr;

    // the pattern defines the relevant tags that are evaluated
    const char* pattern[]={"VolumeCount","Endian","VolumeSize","VolumePosition","VolumeScale","Field","ModelMatrix","##"};
    QString s,*ps;
    QString command;
    _info._littleEndian=true;
    quint16 blkCount=0;

    QFile f(fname);
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        int befNr=-1;
        // we always assume INT16 bit for the moment
        _info._dataType = _optionLsts["data type"].indexOf("int16");

        do
        {
            befNr=-1;
            s=t.readLine();
            if (s.contains("##")) ++blkCount;
            command=s.section(" ",0,0);
            s=s.right(s.length()-command.length()-1);

            for (int i=0;i<8;++i)
            {
                if (command.compare(pattern[i])==0)
                {
                    befNr=i;
                }
            }

            switch (befNr)
            {
            case 0 : // VolumeCount, only one object per file is supported
                if (s.toInt()!=1)
                {
                        ps=new QString();
                        *ps=QString("Error: only one object per file is supported by now. Others will be ignored!");
                }
                break;
            case 1 : // Endian
                _info._littleEndian=s.toLower()=="l";
                break;
            case 2 : // VolumeSize
                _info._dim[0]=s.section(" ",0,0).toInt();
                _info._dim[1]=s.section(" ",1,1).toInt();
                _info._dim[2]=s.section(" ",2,2).toInt();
                break;
            case 3 : // VolumePosition .. not Used in the moment
                _info._pos[0]=s.section(" ",0,0).toInt();
                _info._pos[1]=s.section(" ",1,1).toInt();
                _info._pos[2]=s.section(" ",2,2).toInt();
                break;
            case 4 : // VolumeScale
                _info._vs[0]=s.section(" ",0,0).toFloat();
                _info._vs[1]=s.section(" ",1,1).toFloat();
                _info._vs[2]=s.section(" ",1,1).toFloat();
                break;
            case 5 : // Field
                {
                    int oNr=s.section(" ",0,0).toInt();
                    s=s.section(" ",1,1);
                    s=s.mid(1,s.length()-2);
                }
                break;
            case 6 : // ModelMatrix not Used by now
                s=s.mid(1,s.length()-2);
                break;
            case 7 :
                break;
            }
        }
        while (!t.atEnd() && blkCount<3);

        vtkImageReader* pDataImporter=vtkImageReader::New();
        _info._littleEndian ? pDataImporter->SetDataByteOrderToLittleEndian() : pDataImporter->SetDataByteOrderToBigEndian();
        pDataImporter->SetFileName(fname.toLocal8Bit().constData());
        pDataImporter->SetFileDimensionality(3);
        pDataImporter->SetDataScalarTypeToShort();
        pDataImporter->SetNumberOfScalarComponents(1);
        pDataImporter->SetDataExtent(0,_info._dim[0]-1,0,_info._dim[1]-1,0,_info._dim[2]-1);
        pDataImporter->SetDataSpacing(_info._dim[0]*_info._vs[0],_info._dim[1]*_info._vs[1],_info._dim[2]*_info._vs[2]);
        pDataImporter->SetDataOrigin(_info._pos[0],_info._pos[1],_info._pos[2]);
        _info._headerSkip = f.size()-_info._dim[0]*_info._dim[1]*_info._dim[2]*2.0;
        pDataImporter->SetHeaderSize(_info._headerSkip);
        emit KSignal(ST_SET_PROCESS_RANGE,
                     new QPoint(
                         pDataImporter->GetProgressMinValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value,
                         pDataImporter->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value));
        emit KSignal(ST_SET_PROCESS_TXT,new QString("loading ... "));
        pDataImporter->SetProgressObserver(pProgObserver);
        pDataImporter->Update();
        vtkImageData *pImageData = pDataImporter->GetOutput();
        pImageData->SetSpacing(_info._vs[0],_info._vs[1],_info._vs[2]);
        return pImageData;
    }
    else
    {
        emit KSignal(ST_ERROR_MSG,new QString(QString("file %1 could not be opened").arg(fname)));
        _noError = false;
    }
    return pResult;
}

void xVIOVOX::write(vtkImageDataPtr pData,const QString& fname)
{
    if (pData)
    {
        QFile f(fname);
        if (f.open(QFile::WriteOnly))
        {
            QTextStream t(&f);

            t << "xVTK exported " << QDateTime::currentDateTime().toString("yyMMdd_hh:mm:ss:zzz") << endl;
            t << "##" << endl;
            t << "##" << endl;
            t << "Endian L" << endl;
            int dim[3];pData->GetDimensions(dim);
            t << QString("VolumeSize %1 %2 %3").arg(dim[0]).arg(dim[1]).arg(dim[2]) << endl;
            t << QString("VoxelSize %1").arg(pData->GetScalarSize()*8) << endl;
            double pos[3];pData->GetOrigin(pos);
            t << QString("VolumePosition %1 %2 %3").arg(pos[0]).arg(pos[1]).arg(pos[2]);
            double vs[3];pData->GetSpacing(vs);
            t << QString("VolumeScale %1 %2 %3").arg(vs[0]).arg(vs[1]).arg(vs[2]) << endl;
            t << "##" << endl;

            QDataStream d(&f);

            emit KSignal(ST_SET_PROCESS_RANGE,new QPoint(0,dim[2]-1));
            emit KSignal(ST_SET_PROCESS_TXT,new QString("writing ..."));
            for (long i=0;i<dim[2];++i)
            {
                d.writeRawData((const char*)((quint64)pData->GetScalarPointer()+i*dim[0]*dim[1]*pData->GetScalarSize()),
                        dim[0]*dim[1]*pData->GetScalarSize());
                emit KSignal(ST_SET_PROCESS,new int(i));
            }

            f.close();
        }
        else
        {
            _noError = false;
            emit KSignal(ST_ERROR_MSG,new QString(QString("Couldn't open %1 for writing").arg(fname)));
        }
    }
    else
    {
        _noError = false;
        emit KSignal(ST_ERROR_MSG,new QString("Empty data set can not be exported"));
    }
}
