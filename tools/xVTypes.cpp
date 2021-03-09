#include "xVTypes.h"
#include <QDateTime>
#include <QUrl>
#include <QLocale>
#include <QRect>
#include <QRectF>
#include <QPoint>
#include <QPointF>
#include <QLine>
#include <QLineF>
#include <QRegularExpression>
#include <QUuid>
#include <QEasingCurve>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QFont>
#include <QPixmap>
#include <QColor>
#include <QBrush>
#include <QPalette>
#include <QPen>
#include <QBitmap>
#include <QCursor>
#include <QImage>
#include <QKeySequence>
#include <QTextLength>
#include <QTextFormat>
#include <QMatrix>
#include <QTransform>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QPolygonF>
#include <QIcon>

QMap<QString,xPROP_TYPE> _settings;
QMap<QString,xPROP_TYPE> _globalNameSpace;
QMap<QString,QStringList> _optionLsts;
long long _objIDCount=0;

xAbstractBasisObj::xAbstractBasisObj():QObject(){
    _selected=false;
    _id=QDateTime::currentDateTime().toString("yyMMddhhmmsszzz")+QString("_%1").arg(::_objIDCount++);}

xAbstractBasisObj::xAbstractBasisObj(QDataStream& d):QObject()
{
    d >> _selected;
    d >> _type;
    quint16 _count; d >> _count;
    for (int i=0;i<_count;++i)
    {
        QString k;d >> k;
        xPROP_TYPE prop(d);
        _paramMp[k]=prop;
    }
    d >> _id;
}

void xAbstractBasisObj::setParamSelected(bool b){_selected=b;}

xVProgressObserver::xVProgressObserver(QObject *pParent):QObject(pParent),vtkProgressObserver()
{}

void xVProgressObserver::UpdateProgress(double amount){
    emit KSignal(ST_SET_PROCESS,new int(amount*::_settings["progress scaling factor"]._value.toFloat()));
}

QDataStream &operator<<(QDataStream &out, const QFileInfo &myObj){
    out << myObj.absoluteFilePath();
    return out;}
QDataStream &operator>>(QDataStream &in, QFileInfo &myObj){
    QString s;
    in >> s;myObj.setFile(s);
    return in;}
QDataStream &operator<<(QDataStream &out, const QPointF &myObj){
    out << myObj.x() << myObj.y();
    return out;}
QDataStream &operator>>(QDataStream &in, QPointF &myObj){
    qreal x,y;
    in >> x >> y;myObj=QPointF(x,y);
    return in;}
QDataStream &operator<<(QDataStream &out, const x3D_SAMPLE_POS &myObj){
    out << myObj.x << myObj.y << myObj.z;
    return out;}
QDataStream &operator>>(QDataStream &in, x3D_SAMPLE_POS &myObj){
    in >> myObj.x >> myObj.y >> myObj.z;
    return in;}
QDataStream &operator<<(QDataStream &out, const vtkPiecewiseFunctionPtr &myObj){
    out << (quint16)myObj->GetSize();
    double val[4];
    for (int i=0;i<myObj->GetSize();++i)
    {
        myObj->GetNodeValue(i,val); out << val[0] << val[1] << val[2] << val[3];
    }
    return out;
}
QDataStream &operator>>(QDataStream &in, vtkPiecewiseFunctionPtr &myObj)
{
    quint16 size; in >> size;
    myObj=vtkPiecewiseFunction::New();
    double val[4];
    for (int i=0;i<size;++i)
    {
        in >> val[0] >> val[1] >> val[2] >> val[3];
        myObj->SetNodeValue(i,val);
    }
    return in;
}
QDataStream &operator<<(QDataStream &out, const vtkColorTransferFunctionPtr &myObj)
{
    out << (quint16)myObj->GetSize();
    double pos,col[3];
    for (int i=0;i<myObj->GetSize();++i)
    {
        myObj->GetColor(pos,col); out << pos << col[0] << col[1] << col[2];
    }
    return out;
}
QDataStream &operator>>(QDataStream &in, vtkColorTransferFunctionPtr &myObj)
{
    quint16 size;
    double pos,col[3];
    in >> size;
    myObj=vtkColorTransferFunction::New();
    for (int i=0;i<size;++i)
    {
        in >> pos >> col[0] >> col[1] >> col[2];
        myObj->AddRGBPoint(pos,col[0],col[1],col[2]);
    }
}
QDataStream &operator<<(QDataStream &out, const xLimitedInt &myObj){
    out << myObj._value << myObj._lowerLimit << myObj._upperLimit;
    return out;}
QDataStream &operator>>(QDataStream &in, xLimitedInt &myObj){
    in >> myObj._value >> myObj._lowerLimit >> myObj._upperLimit;
    return in;}
QDataStream &operator<<(QDataStream &out, const xLimitedDouble &myObj){
    out << myObj._value << myObj._lowerLimit << myObj._upperLimit << myObj._precision;
    return out;}
QDataStream &operator>>(QDataStream &in, xLimitedDouble &myObj){
    in >> myObj._value >> myObj._lowerLimit >> myObj._upperLimit >> myObj._precision;
    return in;}
QDataStream &operator<<(QDataStream &out, const xParamMap &myObj){
    out << (quint32)myObj.count();
    for (xParamMap::const_iterator it=myObj.begin();it!=myObj.end();++it)
        out << it.key() << it.value()._id << it.value()._value << it.value()._subGrp;
    return out;
}
QDataStream &operator>>(QDataStream &in, xParamMap &myObj)
{
    quint32 count;
    in >> count;
    for (long i=0;i<count;++i)
    {
        QString key;
        xPROP_TYPE prop;
        in >> key >> prop._id >> prop._value >> prop._subGrp;
        myObj[key]=prop;
    }
    return in;
}


void saveToStream(QDataStream &d,const QVariant& v)
{
    d << v;
}
