#include "xVIOVFF.h"
#include <QTextStream>

vtkImageDataPtr xVIOVFF::read(const QString& fname)
{
    vtkImageDataPtr pResult = nullptr;
    // the pattern defines the relevant tags that are evaluated
    QString s,command;
    _info._littleEndian=false;

    QFile f(fname);
    if (f.open(QFile::ReadOnly))
    {
        // ask scry
        QTextStream t(&f);
        const char* pattern[]={"size","spacing","elementsize","min","max","bits","cmdLine","","rank"};

        int befNr=-1;
        bool abb=false;
        bool _isVolumeData=false;
        do
        {
            befNr=-1;
            s=t.readLine();
            command=s.section("=",0,0);
            s=s.right(s.length()-command.length()-1);
            s=s.left(s.length()-1);

            for (int i=0;i<9;++i)
            {
                if (command.compare(pattern[i])==0)
                {
                    befNr=i;
                }
            }

            switch (befNr)
            {
            case 0 :
                _info._dim[0]=s.section(" ",0,0).toUShort();
                _info._dim[1]=s.section(" ",1,1).toUShort();
                _info._dim[2]=s.section(" ",2,2).toUShort();
                break;
            case 1 :
                _info._vs[0]=s.section(" ",0,0).toFloat();
                _info._vs[1]=s.section(" ",1,1).toFloat();
                _info._vs[2]=s.section(" ",2,2).toFloat();
                break;
            case 2 :
                _info._vs[0]*=s.toFloat();
                _info._vs[1]*=s.toFloat();
                _info._vs[2]*=s.toFloat();
                break;
            case 5 : _info._bytesPerVoxel=s.toInt()/8;break;
            case 7 : abb=true;break;
            case 8 : _isVolumeData=s.section(" ",0,0).toInt()==3;break;
            }
        }
        while (!t.atEnd() && !abb);

        vtkImageReader* pDataImporter=vtkImageReader::New();
        _info._littleEndian ? pDataImporter->SetDataByteOrderToLittleEndian() : pDataImporter->SetDataByteOrderToBigEndian();
        pDataImporter->SetFileName(fname.toLocal8Bit().constData());
        pDataImporter->SetFileDimensionality(3);
        switch (_info._bytesPerVoxel)
        {
        case 1 :
            pDataImporter->SetDataScalarTypeToChar();
            break;
        case 2 :
            pDataImporter->SetDataScalarTypeToShort();
            break;
        }
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

// vff by default is big endian
void xVIOVFF::write(vtkImageDataPtr pData,const QString& fname)
{
    if (pData)
    {
        QFile f(fname);
        if (f.open(QFile::WriteOnly))
        {
            QTextStream t(&f);
            t << "ncaa" << Qt::endl;
            t << "rank=3;" << Qt::endl;
            t << "type=raster;" << Qt::endl;
            int dim[3];pData->GetDimensions(dim);
            t << QString("size=%1 %2 %3;").arg(dim[0]).arg(dim[1]).arg(dim[2]) << Qt::endl;
            double pos[3];pData->GetOrigin(pos);
            t << QString("origin=%1 %2 %3;").arg(pos[0]).arg(pos[1]).arg(pos[2]);
            t << "bands=1;" << Qt::endl;
            t << QString("bits=%1;").arg(pData->GetScalarSize()*8) << Qt::endl;
            t << "format=slice;" << Qt::endl;
            t << "title=xVTK_generated at " << QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss") << ";" << Qt::endl;
            t << "reverse_order=no;" << Qt::endl;
            double range[2];pData->GetScalarRange(range);
            t << QString("min=%1;").arg(range[0]) << Qt::endl;
            t << QString("max=%1;").arg(range[1]) << Qt::endl;
            t << "spacing=1.00 1.00 1.00;" << Qt::endl;
            double vs[3];pData->GetSpacing(vs);
            t << QString("elementsize=%1;").arg(vs[0]) << Qt::endl;

            QDataStream d(&f);
            d.setByteOrder(QDataStream::BigEndian);

            emit KSignal(ST_SET_PROCESS_RANGE,new QPoint(0,dim[2]-1));
            emit KSignal(ST_SET_PROCESS_TXT,new QString("writing ..."));

            if (pData->GetScalarSize()==2)
            {
                for (long i=0;i<dim[2];++i)
                {
                    for (long j=0;j<dim[1];++j)
                        for (long k=0;k<dim[0];++k)
                            d << (quint16)pData->GetScalarComponentAsFloat(k,j,i,0);

                    emit KSignal(ST_SET_PROCESS,new int(i));
                }
            }
            else
            {
                for (long i=0;i<dim[2];++i)
                {
                    d.writeRawData((const char*)((quint64)pData->GetScalarPointer()+i*dim[0]*dim[1]*pData->GetScalarSize()),
                            dim[0]*dim[1]*pData->GetScalarSize());
                    emit KSignal(ST_SET_PROCESS,new int(i));
                }

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
