#include "xVImageObj.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageReader2.h"
#include "vtkInformation.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/dipixel.h"
#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimgle/dimo1img.h"
#include "dcmtk/dcmimgle/dimo2img.h"

#include <stdlib.h>

xVImageObj::xVImageObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_IMAGE;
    _paramMp["file name"]._id=1;
    _paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"undefined"));

    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value=QVariant::fromValue((vtkImageDataPtr)nullptr);
}
xVImageObj::xVImageObj(QDataStream& d):xVGenImpObj(d)
{
    _type = xVOT_IMAGE;

    _outputParamMp["image"]._id=1;
    _outputParamMp["image"]._value=QVariant::fromValue((vtkImageDataPtr)nullptr);
}

vtkImageDataPtr xVImageObj::tryDCMImport(const QString& fname)
{
    vtkImageDataPtr result = nullptr;
    DicomImage *image = new DicomImage(fname.toLatin1());
    if (image != nullptr)
    {
      if (image->getStatus() == EIS_Normal)
      {
/*
          image->

          if (image->isMonochrome())
        {
          image->setMinMaxWindow();
          Uint8 *pixelData = (Uint8 *)(image->getOutputData(8));
          if (pixelData != NULL)
          {
            // do something useful with the pixel data
          }
        }
      */
      }
    }
    return result;
}

void xVImageObj::run()
{
    if (_status == OS_UPDATE_NEEDED)
    {
        vtkImageReader2Factory *readerFactory=vtkImageReader2Factory::New();
        vtkImageReader2 *imageReader=readerFactory->CreateImageReader2(_paramMp["file name"]._value.value<xFileName>()._fileName.toLocal8Bit());
        vtkImageDataPtr vtkImage=nullptr;

        if (imageReader && imageReader->CanReadFile(_paramMp["file name"]._value.value<xFileName>()._fileName.toLocal8Bit()))
        {
            imageReader->SetFileName(_paramMp["file name"]._value.value<xFileName>()._fileName.toLocal8Bit());
            imageReader->Update();
            vtkImageDataPtr vtkImage=vtkImageData::New();
            vtkImage->DeepCopy(imageReader->GetOutput());
            setStatus(OS_VALID);
        }
        else
        {
            QFileInfo info(_paramMp["file name"]._value.value<xFileName>()._fileName);
            if (!vtkImage) vtkImage=tryDCMImport(info.absoluteFilePath());

            if (!vtkImage)
            {
                setStatus(OS_ERROR);
                emit KSignal(ST_ERROR_MSG,new QString(QString("The file %1 cannot be imported as image.").arg(_paramMp["file name"]._value.value<QFileInfo>().absoluteFilePath())));
            }
        }

        readerFactory->Delete();
        imageReader->Delete();

        _outputParamMp["image"]._id=1;
        _outputParamMp["image"]._value=QVariant::fromValue(vtkImage);

    }
}
void xVImageObj::reset()
{
    // unload image
    setStatus(OS_UPDATE_NEEDED);
}

QImage convertFloatVtkTo16bitQImage(vtkImageDataPtr vtkImage)
{
    int dim[4];
    vtkIdType inc[3];
    vtkImage->GetDimensions((int*)dim);
    vtkImage->GetIncrements(inc);


    QImage res(dim[0],dim[1],QImage::Format_Grayscale16);
    // 1st pass get min and max
    double minGVal,maxGVal;
    minGVal = vtkImage->GetScalarComponentAsDouble(0,0,0,0);
    maxGVal = minGVal;
    for (long x=0;x<dim[0];++x)
        for (long y=0;y<dim[1];++y)
        {
            minGVal = std::min(minGVal,vtkImage->GetScalarComponentAsDouble(x,y,0,0));
            maxGVal = std::max(maxGVal,vtkImage->GetScalarComponentAsDouble(x,y,0,0));
        }

    double scaling = 65535.0/(maxGVal-minGVal);

    // 2nd pass encoding
    for (long x=0;x<dim[0];++x)
        for (long y=0;y<dim[1];++y)
        {
            ((ushort*)res.scanLine(y))[x]=(ushort)(vtkImage->GetScalarComponentAsDouble(x,y,0,0)-minGVal)*scaling;
        }
    return res;
}

QImage xVImageObj::convertToQImage(vtkImageDataPtr vtkImage)
{
    // shallow copy
    QImage res;
    if (vtkImage)
    {
        int dim[4];
        vtkIdType inc[3];
        vtkImage->GetDimensions((int*)dim);
        vtkImage->GetIncrements(inc);
        if (vtkImage->GetNumberOfScalarComponents()==1) // monochrome
        {
            switch (vtkImage->GetScalarType())
            {          
            case VTK_SIGNED_CHAR:
            case VTK_CHAR:
            case VTK_UNSIGNED_CHAR:
                // identify mono images
                res = QImage((uchar*)vtkImage->GetScalarPointer(),dim[0],dim[1],inc[1],QImage::Format_Grayscale8);
                break;
            case VTK_SHORT:
            case VTK_UNSIGNED_SHORT:
                res = QImage((uchar*)vtkImage->GetScalarPointer(),dim[0],dim[1],inc[1],QImage::Format_Grayscale16);
                break;
            case VTK_FLOAT:
            case VTK_DOUBLE:
                // down scale to 16bit
                res = convertFloatVtkTo16bitQImage(vtkImage);
                break;
            default:
                // unhandled
                break;
            }
        }
        else //RGB or RGBA
        {
            if (vtkImage->GetNumberOfScalarComponents()==3)
            {
                // RGB
                switch (vtkImage->GetScalarType())
                {
                case VTK_SIGNED_CHAR:
                case VTK_CHAR:
                case VTK_UNSIGNED_CHAR:
                    res = QImage((uchar*)vtkImage->GetScalarPointer(),dim[0],dim[1],inc[1],QImage::Format_RGB888);
                    break;
                case VTK_SHORT:
                case VTK_UNSIGNED_SHORT:
                    res = QImage((uchar*)vtkImage->GetScalarPointer(),dim[0],dim[1],inc[1],QImage::Format_RGB16);
                    break;
                default:
                    // unhandled
                    break;
                }
            }
            else
            {
                // RGBA
                switch (vtkImage->GetScalarType())
                {
                case VTK_SIGNED_CHAR:
                case VTK_CHAR:
                case VTK_UNSIGNED_CHAR:
                    res = QImage((uchar*)vtkImage->GetScalarPointer(),dim[0],dim[1],inc[1],QImage::Format_RGBA8888);
                    break;
                case VTK_SHORT:
                case VTK_UNSIGNED_SHORT:
                    res = QImage((uchar*)vtkImage->GetScalarPointer(),dim[0],dim[1],inc[1],QImage::Format_RGBA64);
                    break;
                default:
                    // unhandled
                    break;
                }
            }
        }
    }

    if (!res.isNull()) res=res.mirrored(false,true);

    return res;
}

