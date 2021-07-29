#include "xVPolyObj.h"
#include <QFileInfo>
#include "xVTypes.h"

xVPolyObj::xVPolyObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_MESH;
    _description="Imports typical mesh files";
    _paramMp["file name"]._id = 1;
    _paramMp["file name"]._value = QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"no file"));
    _paramMp["test"]._id=2;
    _paramMp["test"]._value = 2.0;

    pProgObserver = new xVProgressObserver(this);
    connect(pProgObserver,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SIGNAL( KSignal(const SIG_TYPE&,void*)));

    _outputParamMp["mesh"]._value = QVariant::fromValue((vtkPolyDataMapperPtr)0);
    _outputParamMp["mesh"]._id=0;
}

xVPolyObj::xVPolyObj(QDataStream &d):xVGenImpObj(d)
{
    _type = xVOT_MESH;
    _outputParamMp["mesh"]._value = QVariant::fromValue((vtkPolyDataMapperPtr)0);
    _outputParamMp["mesh"]._id=0;
}

void xVPolyObj::reset()
{
    if (_status!=OS_UPDATE_NEEDED)
    {
        // free all resources
        if (mapper) mapper->Delete();
        mapper=nullptr;
        if (polyData) polyData->Delete();
        polyData = nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
}

vtkPolyData* xVPolyObj::ReadPolyData(const char* fileName)
{
  vtkPolyData* polyData=vtkPolyData::New();
  std::string extension =
      vtksys::SystemTools::GetFilenameLastExtension(std::string(fileName));

  // Drop the case of the extension
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);

  if (extension == ".ply")
  {
    vtkPLYReader *reader=vtkPLYReader::New();
    reader->SetFileName(fileName);
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     reader->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     reader->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    reader->SetProgressObserver(pProgObserver);

    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkXMLPolyDataReader *reader=vtkXMLPolyDataReader::New();
    reader->SetFileName(fileName);
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     reader->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     reader->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    reader->SetProgressObserver(pProgObserver);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkOBJReader *reader=vtkOBJReader::New();
    reader->SetFileName(fileName);
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     reader->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     reader->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    reader->SetProgressObserver(pProgObserver);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkSTLReader* reader=vtkSTLReader::New();
    reader->SetFileName(fileName);
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     reader->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     reader->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    reader->SetProgressObserver(pProgObserver);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkPolyDataReader* reader=vtkPolyDataReader::New();
    reader->SetFileName(fileName);
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     reader->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     reader->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    reader->SetProgressObserver(pProgObserver);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkBYUReader* reader=vtkBYUReader::New();
    reader->SetGeometryFileName(fileName);
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     reader->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     reader->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    reader->SetProgressObserver(pProgObserver);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkSphereSource* source=vtkSphereSource::New();
    emit KSignal(ST_SET_PROCESS_RANGE,
                 new QPoint(
                     source->GetProgressMinValue()*::_settings["progress scaling factor"]._value.toFloat(),
                     source->GetProgressMaxValue()*::_settings["progress scaling factor"]._value.toFloat()));
    source->SetProgressObserver(pProgObserver);
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}

void xVPolyObj::run()
{
    xVGenImpObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;
    // test if file name exists
    QFileInfo pInfo(_paramMp["file name"]._value.value<xFileName>()._fileName);
    if (pInfo.isFile() && pInfo.exists())
    {
        setStatus(OS_RUNNING);
        // load data (progress?)
        polyData = ReadPolyData(pInfo.absoluteFilePath().toLocal8Bit().constData());
        //emit KSignal(ST_MSG,new QString(QString("%1 poinst found in data set").arg(polyData->GetNumberOfPoints())));
        if (polyData->GetNumberOfPoints()==0)
        {
            setStatus(OS_ERROR);
            emit KSignal(ST_ERROR_MSG,new QString("no vertexes found in data set"));
            return;
        }
        else
        {
            mapper=vtkPolyDataMapper::New();
            mapper->SetInputData(polyData);

            setStatus(OS_VALID);
            emit KSignal(ST_MSG,new QString("loading successful"));
        }

        _outputParamMp["mesh"]._value = QVariant::fromValue(mapper);
        _outputParamMp["mesh"]._id=0;

    }
    else
    {
        emit KSignal(ST_ERROR_MSG,new QString(QString("%1 file not found").arg(pInfo.absoluteFilePath())));
        setStatus(OS_ERROR);
    }
}

void xVPolyObj::paramModified(const QString &txt)
{
    xVGenImpObj::paramModified(txt);
    // read new data?
}

void xVPolyObj::save(QDataStream &d,bool _explicit)
{
    QFileInfo pInfo=_paramMp["file name"]._value.value<xFileName>()._fileName;
    if (_explicit && pInfo.isFile() && pInfo.exists())
    {
        QFile::copy(pInfo.absoluteFilePath(),_id+"."+pInfo.suffix());
        pInfo.setFile(_id+"."+pInfo.suffix());
        _paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,pInfo.absoluteFilePath(),true));
    }

    xVGenImpObj::save(d,_explicit);
}
