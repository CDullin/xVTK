#ifndef XVTYPES_H
#define XVTYPES_H

#include <QVariant>
#include <QFileInfo>
#include <QMetaType>

#include <vtkActor.h>
#include <vtkImageReader.h>
#include <vtkVolume.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkProgressObserver.h>
#include <vtkPolyDataMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>

#include <QDataStream>

class xVAbstractBaseObj;

//void saveToStream(QDataStream &d,const QVariant& v);
QString objName2objId(const QString& name);
xVAbstractBaseObj* objId2objPtr(const QString& id);

enum VARIANT_TYPES
{
    VT_READ_FILE = QMetaType::User,
    VT_SAVE_FILE = QMetaType::User+1,
    VT_READ_DIR  = QMetaType::User+2,
    VT_SAVE_DIR  = QMetaType::User+3,
    VT_ACTOR     = QMetaType::User+4,
    VT_WDGT_USER_DATA = QMetaType::User+5,
    VT_3D_SAMPLE_POS = QMetaType::User+5
};

struct LIMITS
{
    float _lowerLimit=0.0f;
    float _upperLimit=1.0f;
    float _reduction=1.0f;
};

enum SIG_TYPE
{
    ST_MSG,
    ST_ERROR_MSG,
    ST_WARN_MSG,
    ST_ABOUT_TO_CLOSE,
    ST_ADD_OBJECT,
    ST_OBJECT_ADDED,
    ST_REMOVE_OBJECT,
    ST_OBJECT_REMOVED,
    ST_OBJECT_ACTIVATED,
    ST_SET_ALL_PROCESS_RANGE,
    ST_SET_ALL_PROCESS,
    ST_INC_ALL_PROCESS,
    ST_SET_ALL_PROCESS_TXT,
    ST_RESET_ALL_PROCESS,
    ST_SET_PROCESS_RANGE,
    ST_SET_PROCESS,
    ST_INC_PROCESS,
    ST_SET_PROCESS_TXT,
    ST_RESET_PROCESS,
    ST_HISTO_UPDATED,
    ST_ADD_ACTION_HOOKS,
    ST_ACTION_HOOK_TRIGGERED,
    ST_CREATE_OBJ_HOOK,
    ST_GENERATE_CONNECTION_WITH_MAIN_DLG,
    ST_ADD_VISUALIZATION_WDGT,
    ST_REMOVE_VISUALIZATION_WDGT,
    ST_GLOBAL_NAMESPACE_MODIFIED
};

enum xVO_TYPE
{
    xVOT_INVALID            = 0xFFFFFFFF,
    xVOT_END                = 0xEEEEEEEE,
    xVOT_START              = 0x00000000,
    xVOT_DATA               = 0x00000100,                   // this flag indicates that this is a data import type
    xVOT_VOLUME             = 0x00000101,
    xVOT_MESH               = 0x00000102,
    xVOT_IMAGE              = 0x00000103,
    xVOT_IMAGE_STACK        = 0x00000104,
    xVOT_CVS                = 0x00000105,
    xVOT_PLANE              = 0x00000200,
    xVOT_VIEW               = 0x00000400,
    xVOT_2D_VIEW            = 0x00000401,
    xVOT_3D_VIEW            = 0x00000402,
    xVOT_FILTER             = 0x00000800,
    xVOT_USER_DLG           = 0x00001000,
    xVOT_USER_TABLE_DLG     = 0x00001001,
    xVOT_CONNECTOR          = 0x00002000,
    xVOT_VIS_PROP           = 0x00004000,
    xVOT_VOLUME_VIS_PROP    = 0x00004001,
    xVOT_MESH_VIS_PROP      = 0x00004002,
    xVOT_CLIPPING_OBJ       = 0x00008000,
    xVOT_CLIPPING_PLANE     = 0x00008001,
    xVOT_CLIPPING_BOX       = 0x00008002,
    xVOT_CLIPPING_SPHERE    = 0x00008003,
    xVOT_TOOL_OBJ           = 0x00008100,
    xVOT_SLICER_OBJ         = 0x00008101,
    xVOT_CALL_EXTERNAL      = 0x00010001,
    xVOT_LOOP_COUNTER       = 0x00020001,
    xVOT_IF                 = 0x00020002,
    xVOT_TRAFFIC_LIGHT      = 0x00020003,
    xVOT_MATH               = 0x00020004,
    xVOT_WAIT               = 0x00010005,
    xVOT_VAR_DEFINITION     = 0x00010104,
    xVOT_ARDUINO_CONNECT    = 0x00040000,
    xVOT_ARDUINO_COM        = 0x00040001
};

struct HOOK
{
public:
    QString _rubrik;
    QString _name;
    SIG_TYPE _triggeredAction;
    xVO_TYPE _objType=xVOT_INVALID;
    HOOK(const QString& r,const QString& t,const SIG_TYPE& a,const xVO_TYPE& ot=xVOT_INVALID)
    {
        _rubrik = r;
        _name = t;
        _triggeredAction = a;
        _objType = ot;
    }
};


enum xVSCALAR_TYPE
{
    ST_CHAR     = 0x00,
    ST_UCHAR    = 0x01,
    ST_SHORT    = 0x02,
    ST_USHORT   = 0x03,
    ST_INT      = 0x04,
    ST_UINT     = 0x05,
    ST_FLOAT    = 0x06,
    ST_DOUBLE   = 0x07
};

struct xPROP_TYPE
{
public:
    quint32 _id=0;
    QVariant _value;
    QString  _subGrp="";
    QString  _subGrpIDStr="";
    xVAbstractBaseObj *pRefObj=nullptr;

    void save(QDataStream &d);
    xPROP_TYPE(){};
    xPROP_TYPE(const xPROP_TYPE& other){
        _id=other._id;
        _value=other._value;
        _subGrp=other._subGrp;
        pRefObj=other.pRefObj;
    }
    xPROP_TYPE(QDataStream& d)
    {
        d >> _id;
        d >> _value;
        d >> _subGrp;
        d >> _subGrpIDStr;
    }
};

struct xLimitedInt
{
public:
    xLimitedInt(){}
    xLimitedInt(const quint32& v, const quint32& l, const quint32& h){_value=v;_lowerLimit=l;_upperLimit=h;}
    xLimitedInt(const xLimitedInt& other){_value=other._value;_lowerLimit=other._lowerLimit;_upperLimit=other._upperLimit;}
    qint32 _value=0,_lowerLimit=0,_upperLimit=0;
    void setValue(const qint32& v){_value=v;}
    void setLowerLimit(const qint32& v){_lowerLimit=v;}
    void setUpperLimit(const qint32& v){_upperLimit=v;}
};

struct xLimitedDouble
{
public:
    xLimitedDouble(){}
    xLimitedDouble(const double& v,const double& l,const double& u,const double& p){
        _value=v;_lowerLimit=l;_upperLimit=u;_precision=p;}
    xLimitedDouble(const xLimitedDouble& other){
        _value=other._value;_lowerLimit=other._lowerLimit;_upperLimit=other._upperLimit;_precision=other._precision;}
    double _value=0.0,_lowerLimit=0.0,_upperLimit=0.0,_precision=0.0;
};

struct x3D_SAMPLE_POS
{
public:
    quint32 x,y,z;
    x3D_SAMPLE_POS(){x=y=z=0;}
    x3D_SAMPLE_POS(const long& v){x=y=z=v;}
    x3D_SAMPLE_POS(const long&xx,const long& yy,const long& zz){x=xx;y=yy;z=zz;}
};

Q_DECLARE_METATYPE(x3D_SAMPLE_POS);

class xVProgressObserver:public QObject,public vtkProgressObserver
{
    Q_OBJECT
public:
    xVProgressObserver(QObject* parent=nullptr);
    virtual void UpdateProgress(double amount) override;
signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
};

typedef vtkActor* vtkActorPtr;
Q_DECLARE_METATYPE(vtkActorPtr);
typedef vtkVolume* vtkVolumePtr;
Q_DECLARE_METATYPE(vtkVolumePtr);
typedef vtkPolyData* vtkPolyDataPtr;
Q_DECLARE_METATYPE(vtkPolyDataPtr);
typedef vtkImageReader* vtkImageReaderPtr;
Q_DECLARE_METATYPE(vtkImageReaderPtr);
typedef vtkImageData* vtkImageDataPtr;
Q_DECLARE_METATYPE(vtkImageDataPtr);
typedef vtkPolyDataMapper* vtkPolyDataMapperPtr;
Q_DECLARE_METATYPE(vtkPolyDataMapperPtr);
typedef vtkPiecewiseFunction* vtkPiecewiseFunctionPtr;
Q_DECLARE_METATYPE(vtkPiecewiseFunctionPtr);
typedef vtkColorTransferFunction* vtkColorTransferFunctionPtr;
Q_DECLARE_METATYPE(vtkColorTransferFunctionPtr);
typedef QFileInfo* QFileInfoPtr;
Q_DECLARE_METATYPE(QFileInfoPtr);
typedef QList <HOOK>* HOOKLstPtr;
Q_DECLARE_METATYPE(xLimitedInt);
Q_DECLARE_METATYPE(xLimitedDouble);
typedef QMap<QString,xPROP_TYPE> xParamMap;
Q_DECLARE_METATYPE(xParamMap);

QDataStream &operator<<(QDataStream &out, const QFileInfo &myObj);
QDataStream &operator>>(QDataStream &in, QFileInfo &myObj);
QDataStream &operator<<(QDataStream &out, const QPointF &myObj);
QDataStream &operator>>(QDataStream &in, QPointF &myObj);
QDataStream &operator<<(QDataStream &out, const x3D_SAMPLE_POS &myObj);
QDataStream &operator>>(QDataStream &in, x3D_SAMPLE_POS &myObj);
QDataStream &operator<<(QDataStream &out, const vtkPiecewiseFunctionPtr &myObj);
QDataStream &operator>>(QDataStream &in, vtkPiecewiseFunctionPtr &myObj);
QDataStream &operator<<(QDataStream &out, const vtkColorTransferFunctionPtr &myObj);
QDataStream &operator>>(QDataStream &in, vtkColorTransferFunctionPtr &myObj);
QDataStream &operator<<(QDataStream &out, const xLimitedInt &myObj);
QDataStream &operator>>(QDataStream &in, xLimitedInt &myObj);
QDataStream &operator<<(QDataStream &out, const xLimitedDouble &myObj);
QDataStream &operator>>(QDataStream &in, xLimitedDouble &myObj);
QDataStream &operator<<(QDataStream &out, const xParamMap &myObj);
QDataStream &operator>>(QDataStream &in, xParamMap &myObj);

extern QMap<QString,LIMITS> _limitMp;
extern QMap<QString,xPROP_TYPE> _settings;
extern QMap<QString,xPROP_TYPE> _globalNameSpace;
extern QMap<QString,QStringList> _optionLsts;
extern long long _objIDCount;
extern QList <xVAbstractBaseObj*> _objLst;

#endif // XVTYPES_H
