#include "xvtkdlg.h"
#include "xvEvalCondition.h"
#include "QVTKApplication.h"

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
    QVTKApplication a(argc, argv);
    xVTKDlg w;
    w.show();
    a.setPalette(w.palette());
    return a.exec();
}
