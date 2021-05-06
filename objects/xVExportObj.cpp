#include "xVExportObj.h"
#include "vtkImageWriter.h"
#include "vtkPNGWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkBMPWriter.h"
#include "vtkMetaImageWriter.h"
#include "vtkPLYWriter.h"
#include "vtkSTLWriter.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "xVIOVOX.h"
#include "xVIOVFF.h"

/* implement volume export
 * vff, vox, raw (scry)
 */

xVExportObj::xVExportObj(const QString& txt):xVGenVisObj(txt)
{
    _type = xVOT_EXPORT;
    _maxInput = 1;
    _paramMp["file name"]._id=1;
    _paramMp["file name"]._value = QVariant::fromValue(xFileName(xFileName::FN_OUTPUT_FILE,""));
}

xVExportObj::xVExportObj(QDataStream &d):xVGenVisObj(d)
{
    _type = xVOT_EXPORT;
    _maxInput = 1;
}

bool xVExportObj::exportImage(vtkImageDataPtr pImageData)
{
    bool _noError = true;
    QFileInfo info(_paramMp["file name"]._value.value<xFileName>()._fileName);

    if (pImageData==nullptr) return 0;
    int dim[3];pImageData->GetDimensions(dim);

    if (dim[2]<2)
    {

        QStringList pattern;
        pattern << "PNG" << "TIF" << "TIFF" << "JPG" << "JPEG" << "BMP" << "MHD" << "MHA";
        vtkImageWriter *pWriter = nullptr;
        switch (pattern.indexOf(info.suffix().toUpper()))
        {
        case 0 : // PNG
            pWriter = vtkPNGWriter::New();
            break;
        case 1 : // TIF
        case 2 : // TIFF
            pWriter = vtkTIFFWriter::New();
            break;
        case 3 : // JPG
        case 4 : // JPEG
            pWriter = vtkJPEGWriter::New();
            break;
        case 5 : // BMP
            pWriter = vtkTIFFWriter::New();
            break;
        case 6 : // MHD
        case 7 : // MHA
            pWriter = vtkMetaImageWriter::New();
            break;
        }

        if (pWriter)
        {
            pWriter->SetInputData(pImageData);
            pWriter->SetFileName(info.absoluteFilePath().toLatin1());
            emit KSignal(ST_SET_PROCESS_RANGE,
                         new QPoint(
                             pWriter->GetProgressMinValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value,
                             pWriter->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value));
            emit KSignal(ST_SET_PROCESS_TXT,new QString("writing ... "));
            pWriter->SetProgressObserver(pProgObserver);
            pWriter->Update();

        }
        else
        {
            emit KSignal(ST_ERROR_MSG,new QString("filename suffix did not specified a valid exporter plugin"));
            _noError = false;
        }
    }
    else
    {
        QStringList pattern;
        pattern << "VOX" << "VFF" << "VOL" << "RAW";
        vtkWriter *pWriter = nullptr;
        switch (pattern.indexOf(info.suffix().toUpper()))
        {
        case 0 : // VOX
            {
                xVIOVOX exporter(this);
                connect(&exporter,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
                exporter.write(pImageData,info.absoluteFilePath());
            }
            break;
        case 1 : // VFF
            {
                xVIOVFF exporter(this);
                connect(&exporter,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
                exporter.write(pImageData,info.absoluteFilePath());
            }
            break;
        case 2 : // VOL
        case 3 : // RAW
            emit KSignal(ST_WARN_MSG,new QString("exporter not implemented yet"));
        break;
        }
    }
    return _noError;
}

bool xVExportObj::exportVolume(vtkVolumePtr pVolumeData)
{
    bool _noError = true;

    QFileInfo info(_paramMp["file name"]._value.value<xFileName>()._fileName);

    QStringList pattern;
    pattern << "VOX" << "VFF" << "VOL" << "RAW";
    vtkWriter *pWriter = nullptr;
    switch (pattern.indexOf(info.suffix().toUpper()))
    {
    case 0 : // VOX
        {
            xVIOVOX exporter(this);
            connect(&exporter,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));
            vtkFixedPointVolumeRayCastMapper *pMapper = dynamic_cast<vtkFixedPointVolumeRayCastMapper*>(pVolumeData->GetMapper());
            exporter.write((vtkImageDataPtr)pMapper->GetDataSetInput(),info.absoluteFilePath());
        }
        break;
    case 1 : // VFF
    case 2 : // VOL
    case 3 : // RAW
        emit KSignal(ST_WARN_MSG,new QString("exporter not implemented yet"));
    break;
    }

    return _noError;
}

bool xVExportObj::exportMesh(vtkPolyDataMapperPtr pPolyData)
{
    bool _noError = true;

    QFileInfo info(_paramMp["file name"]._value.value<xFileName>()._fileName);

    QStringList pattern;
    pattern << "STL" << "PLY";
    vtkWriter *pWriter = nullptr;
    switch (pattern.indexOf(info.suffix().toUpper()))
    {
    case 0 : // STL
        {
            vtkSTLWriter* pSTLWriter = vtkSTLWriter::New();
            pSTLWriter->SetFileName(info.absoluteFilePath().toLatin1());
            pWriter = pSTLWriter;
        }
        break;
    case 1 : // PLY
        {
            vtkPLYWriter *pPLYWriter = vtkPLYWriter::New();
            pPLYWriter->SetFileName(info.absoluteFilePath().toLatin1());
            pWriter = pPLYWriter;
        }
        break;
    }

    if (pWriter)
    {
        pWriter->SetInputData(pPolyData->GetOutputDataObject(0));
        emit KSignal(ST_SET_PROCESS_RANGE,
                     new QPoint(
                         pWriter->GetProgressMinValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value,
                         pWriter->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.value<xLimitedDouble>()._value));
        emit KSignal(ST_SET_PROCESS_TXT,new QString("writing ... "));
        pWriter->SetProgressObserver(pProgObserver);
        pWriter->Update();

    }
    else
    {
        emit KSignal(ST_ERROR_MSG,new QString("filename suffix did not specified a valid exporter plugin"));
        _noError = false;
    }

    return _noError;
}

void xVExportObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        vtkImageDataPtr pImageData = nullptr;
        vtkVolumePtr pVolumeData = nullptr;
        vtkPolyDataMapperPtr pPolyData = nullptr;

        // connect to data
        for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
        {
            // find all connected and enabled inputs
            if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            {
                for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                {
                    for (QMap <QString,xPROP_TYPE>::iterator it3=(*it)->outputParamMap()->begin();
                         it3!=(*it)->outputParamMap()->end();++it3)
                    {
                        if (it3->_value.userType()==QMetaType::type("vtkImageDataPtr"))
                            pImageData = it3->_value.value<vtkImageDataPtr>();
                        if (it3->_value.userType()==QMetaType::type("vtkVolumePtr"))
                            pVolumeData = it3->_value.value<vtkVolumePtr>();
                        if (it3->_value.userType()==QMetaType::type("vtkPolyDataMapperPtr"))
                            pPolyData = it3->_value.value<vtkPolyDataMapperPtr>();
                    }
                }
            }
        }

        bool _noError = true;
        if (pImageData) // export image file
            _noError = _noError && exportImage(pImageData);

        if (pVolumeData) // export volume file
            _noError = _noError && exportVolume(pVolumeData);

        if (pPolyData)
            _noError = _noError && exportMesh(pPolyData);

        _noError ? setStatus(OS_VALID) : setStatus(OS_ERROR);
    }
}

void xVExportObj::paramModified(const QString& txt)
{
    xVGenVisObj::paramModified(txt);
}
