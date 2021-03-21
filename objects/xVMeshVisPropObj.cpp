#include "xVMeshVisPropObj.h"
#include "xVPolyObj.h"

#include <vtkProperty.h>

xVMeshVisPropObj::xVMeshVisPropObj(const QString& txt):xVGenVisPropObj(txt)
{
    _type = xVOT_MESH_VIS_PROP;
    _paramMp["mesh visualization style"]._id=1;
    _paramMp["mesh visualization style"]._value="face";

    _paramMp["front diffuse color"]._id=2;
    _paramMp["front diffuse color"]._value   = QVariant(QColor(Qt::blue));
    _paramMp["front diffuse color"]._subGrp="face";
    _paramMp["front specular"]._id=3;
    _paramMp["front specular"]._value   = QVariant(0.6);
    _paramMp["front specular"]._subGrp="face";
    _paramMp["front specular power"]._id=4;
    _paramMp["front specular power"]._value   = QVariant(30.0);
    _paramMp["front specular power"]._subGrp="face";
    _paramMp["back diffuse color"]._id=5;
    _paramMp["back diffuse color"]._value   = QVariant(QColor(Qt::blue));
    _paramMp["back diffuse color"]._subGrp="face";
    _paramMp["back specular"]._id=6;
    _paramMp["back specular"] ._value   = QVariant(0.3);
    _paramMp["back specular"]._subGrp="face";
    _paramMp["back specular power"]._id=7;
    _paramMp["back specular power"]._value   = QVariant(30.0);
    _paramMp["back specular power"]._subGrp="face";

    _paramMp["render points as spheres"]._id=8;
    _paramMp["render points as spheres"]._value= QVariant(true);
    _paramMp["render points as spheres"]._subGrp="points";
    _paramMp["point color"]._id=9;
    _paramMp["point color"]._value  = QVariant(QColor(Qt::blue));
    _paramMp["point color"]._subGrp="points";
    _paramMp["point size"]._id=10;
    _paramMp["point size"]._value  = QVariant(5.0);
    _paramMp["point size"]._subGrp="points";

    _paramMp["render lines as tubes"]._id=11;
    _paramMp["render lines as tubes"]._value=QVariant(true);
    _paramMp["render lines as tubes"]._subGrp="mesh";
    _paramMp["line color"]._id=12;
    _paramMp["line color"]._value = QVariant(QColor(Qt::white));
    _paramMp["line color"]._subGrp = "mesh";
    _paramMp["line thickness"]._id=13;
    _paramMp["line thickness"]._value = QVariant((double)1.3);
    _paramMp["line thickness"]._subGrp = "mesh";
}

xVMeshVisPropObj::xVMeshVisPropObj(QDataStream &d):xVGenVisPropObj(d)
{
    _type = xVOT_MESH_VIS_PROP;
}

void xVMeshVisPropObj::reset()
{
    if (_status!=OS_UPDATE_NEEDED)
    {
        // free all resources
        if (actor) actor->Delete();
        actor=nullptr;
        setStatus(OS_UPDATE_NEEDED);
    }
}

void xVMeshVisPropObj::run()
{
    xVGenVisPropObj::run();
    if (status()!=OS_UPDATE_NEEDED) return;

    setStatus(OS_RUNNING);
    vtkPolyDataMapper *mapper = nullptr;

    for (QList <xConnector*>::iterator it2=connectorLst()->begin();it2!=connectorLst()->end();++it2)
        // find all connected and enabled inputs
        if ((*it2)->type()==xCT_INPUT && (*it2)->isEnabled())
            for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
            {
                xVPolyObj *pIObj = dynamic_cast<xVPolyObj*>(*it);
                if (pIObj && pIObj->outputParamMap()->contains("mesh"))
                    mapper=(*pIObj->outputParamMap())["mesh"]._value.value<vtkPolyDataMapperPtr>();
            }

    if (mapper)
    {

        //backProp=vtkProperty::New();
        actor=vtkActor::New();
        actor->SetMapper(mapper);
        //actor->SetBackfaceProperty(backProp);

        paramModified("");

        _outputParamMp["actor"]._value = QVariant::fromValue(actor);
        _outputParamMp["actor"]._id=0;
        setStatus(OS_VALID);
    }
    else
    {
        emit KSignal(ST_ERROR_MSG,new QString("no input mesh found"));
        setStatus(OS_ERROR);
    }
}

void xVMeshVisPropObj::paramModified(const QString &txt)
{
    xVGenVisPropObj::paramModified(txt);

    if (!actor) return;

    QStringList lst=::_optionLsts["mesh visualization style"];
    switch (lst.indexOf(_paramMp["mesh visualization style"]._value.toString()))
    {
    case 0 : // points
        {
        double col1[3];
        col1[0]=_paramMp["point color"]._value.value<QColor>().redF();
        col1[1]=_paramMp["point color"]._value.value<QColor>().greenF();
        col1[2]=_paramMp["point color"]._value.value<QColor>().blueF();
        actor->GetProperty()->SetRepresentationToPoints();
        actor->GetProperty()->SetRenderPointsAsSpheres(_paramMp["render points as spheres"]._value.toBool());
        actor->GetProperty()->SetPointSize(_paramMp["point size"]._value.toDouble());
        actor->GetProperty()->SetColor(col1);
        actor->GetProperty()->SetBackfaceCulling(false);
        }
        break;
    case 1 : // lines
        {
        double col1[3];
        col1[0]=_paramMp["line color"]._value.value<QColor>().redF();
        col1[1]=_paramMp["line color"]._value.value<QColor>().greenF();
        col1[2]=_paramMp["line color"]._value.value<QColor>().blueF();
        actor->GetProperty()->SetRepresentationToWireframe();
        actor->GetProperty()->SetRenderLinesAsTubes(_paramMp["render lines as tubes"]._value.toBool());
        actor->GetProperty()->SetLineWidth(_paramMp["line thickness"]._value.toDouble());
        actor->GetProperty()->SetColor(col1);
        actor->GetProperty()->SetBackfaceCulling(false);
        }
        break;
    case 2 : // faces
        {
        double col1[3],col2[3];
        col1[0]=_paramMp["front diffuse color"]._value.value<QColor>().redF();
        col1[1]=_paramMp["front diffuse color"]._value.value<QColor>().greenF();
        col1[2]=_paramMp["front diffuse color"]._value.value<QColor>().blueF();
        col2[0]=_paramMp["back diffuse color"]._value.value<QColor>().redF();
        col2[1]=_paramMp["back diffuse color"]._value.value<QColor>().greenF();
        col2[2]=_paramMp["back diffuse color"]._value.value<QColor>().blueF();
        double col3[3];
        col3[0]=_paramMp["line color"]._value.value<QColor>().redF();
        col3[1]=_paramMp["line color"]._value.value<QColor>().greenF();
        col3[2]=_paramMp["line color"]._value.value<QColor>().blueF();
        //backProp->SetDiffuseColor(col1);
        //backProp->SetSpecular(_paramMp["front specular"]._value.toDouble());
        //backProp->SetSpecularPower(_paramMp["front specular power"]._value.toDouble());
        actor->GetProperty()->SetDiffuseColor(col2);
        //actor->GetProperty()->SetSpecular(_paramMp["back specular"]._value.toDouble());
        //actor->GetProperty()->SetSpecularPower(_paramMp["back specular power"]._value.toDouble());
        actor->GetProperty()->SetBackfaceCulling(true);
        actor->GetProperty()->SetRepresentationToSurface();
        actor->GetProperty()->SetEdgeColor(col3[0],col3[1],col3[2]);
        //actor->GetProperty()->EdgeVisibilityOn();
        actor->GetProperty()->SetShading(true);
        actor->GetProperty()->SetOpacity(1.0);
       }
        break;
    }
}

void xVMeshVisPropObj::save(QDataStream &d,bool _explicit)
{
    xVGenVisPropObj::save(d,_explicit);
}
