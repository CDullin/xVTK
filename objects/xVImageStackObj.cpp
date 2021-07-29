#include "xVImageStackObj.h"
#include <QDataStream>
#include <QDir>

xVImageStackObj::xVImageStackObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_IMAGE_STACK;
    _description = "Imports as sequence or stack of images";
    _paramMp["stack source type"]._id=1;
    _paramMp["stack source type"]._value="folder";
    addToList(_optionLsts["stack source type"],QStringList() << "folder" << "volume file");
    _paramMp["stack represents"]._id=2;
    _paramMp["stack represents"]._value="volume";
    addToList(_optionLsts["stack represents"],QStringList() << "volume" << "ensemble" << "time series");
    _paramMp["folder name"]._id=3;
    _paramMp["folder name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_DIR,""));
    _paramMp["folder name"]._subGrp="folder";
    _paramMp["name filter"]._id=4;
    _paramMp["name filter"]._value="*";
    _paramMp["name filter"]._subGrp="folder";
    _paramMp["file name"]._id=5;
    _paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,""));
    _paramMp["file name"]._subGrp="volume file";
    _paramMp["virtual stack"]._id=6;
    _paramMp["virtual stack"]._value=true;
    _paramMp["frame rate"]._id=7;
    _paramMp["frame rate"]._subGrp="time series";
    _paramMp["frame rate"]._value=QVariant::fromValue(xLimitedDouble(30,1,200,10));

    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value=QVariant::fromValue((vtkImageDataPtr)nullptr);
    _outputParamMp["virtual stack"]._id=2;
    _outputParamMp["virtual stack"]._value=true;
    _outputParamMp["volume data"]._id=3;
    _outputParamMp["volume data"]._value=QVariant::fromValue((vtkImageDataPtr)nullptr);
    _outputParamMp["current frame"]._id=4;
    _outputParamMp["current frame"]._value=QVariant::fromValue(xLimitedInt(0,0,99));
}

xVImageStackObj::xVImageStackObj(QDataStream &d):xVGenImpObj(d)
{
    _type = xVOT_IMAGE_STACK;

    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value=QVariant::fromValue((vtkImageDataPtr)nullptr);
    _outputParamMp["virtual stack"]._id=2;
    _outputParamMp["virtual stack"]._value=true;
    _outputParamMp["volume data"]._id=3;
    _outputParamMp["volume data"]._value=QVariant::fromValue((vtkImageDataPtr)nullptr);
    _outputParamMp["current frame"]._id=4;
    _outputParamMp["current frame"]._value=QVariant::fromValue(xLimitedInt(0,0,99));
}

void xVImageStackObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        // load all or load as virtual stack
        if (_paramMp["stack source type"]._value.toString()=="folder")
        {
            QDir d(_paramMp["folder name"]._value.value<xFileName>()._fileName);
            d.setFilter(QDir::Files);
            d.setNameFilters(QStringList() << _paramMp["name filter"]._value.toString());

            if (d.exists())
            {
                _fileLst = d.entryInfoList(QDir::NoFilter,QDir::SortFlag::Name);
                if (_fileLst.count()>0)
                {
                    if (_paramMp["virtual stack"]._value.toBool())
                    {
                        // load only one
                    }
                    else
                    {
                        // load all
                    }
                }
                else
                    emit KSignal(ST_ERROR_MSG,new QString("No images were found"));
            }
            else
            {
                emit KSignal(ST_ERROR_MSG,new QString("Inport directory does not exists"));
            }
        }
        else
        {
            // volume data
        }

        setStatus(OS_VALID);
    }
}

void xVImageStackObj::paramModified(const QString& txt)
{
    xVGenImpObj::paramModified(txt);
    // reload the data?
    // update current slice number
    // send out update signal
}

void xVImageStackObj::reset()
{
    // unload image data
    setStatus(OS_UPDATE_NEEDED);
}
