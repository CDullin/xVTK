#include "xvtkdlg.h"
#include "xvEvalCondition.h"
#include "QVTKApplication.h"

#include <QDir>
#include "xVImageObj.h"

int main(int argc, char *argv[])
{
/*
    ::_globalNameSpace["Var1"]._value = "Klaus Xaver";
    ::_globalNameSpace["Var2"]._value = 90.0f;
    ::_globalNameSpace["Var3"]._value = 3.0f;

    xVObj_Basics *pVObj=new xVObj_Basics();
    (*pVObj->paramMap())["VVar"]._value=3.0f;
    //    bool ok= xVEvalCondition::isTrue("{Var1|0}.contains(\"Xa\") and {Var2|0}.isReal and 0b01000001");
    //    bool ok= xVEvalCondition::isTrue(QString("({VVar|%1}--{Var3|0})==6").arg((qulonglong)pVObj));

    double angle=xVEvalCondition::resultOf("sin({Var2|0})").toDouble();       
*/
/*
    QDir d("/home/heimdall/development/sample_data/test_images");
    d.setFilter(QDir::Files);
    QFileInfoList lst=d.entryInfoList();
    xVImageObj pImgObj;
    for (int i=0;i<lst.count();++i)
    {
        pImgObj.reset();
        (*pImgObj.paramMap())["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,lst[i].absoluteFilePath()));
        pImgObj.run();
        QImage img=pImgObj.convertToQImage((*pImgObj.outputParamMap())["image"]._value.value<vtkImageDataPtr>());
        img.save(lst[i].absolutePath()+"/../conv_images/"+lst[i].fileName());
    }
*/
    QVTKApplication a(argc, argv);
    xVTKDlg w;
    w.show();
    a.setPalette(w.palette());
    return a.exec();
}
