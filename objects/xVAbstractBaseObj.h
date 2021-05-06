#ifndef XVABSTRACTBASEOBJ_H
#define XVABSTRACTBASEOBJ_H

#include "xVTypes.h"

//!
//! \brief The xVAbstractBaseObj class
//! is the general object type to be visualized in the dashboard including connectors
//!
class xVAbstractBaseObj:public QObject
{
    Q_OBJECT
public:
    xVAbstractBaseObj();
    xVAbstractBaseObj(QDataStream& d);
    virtual void setParamSelected(bool b);
    bool isParamSelected(){return _selected;}
    bool hasPreview(){return _hasPreview;}
    virtual QMap<QString,xPROP_TYPE>* paramMap(){return &_paramMp;}
    quint32 type(){return _type;}
    QString id(){return _id;}
    virtual void save(QDataStream& d,bool _explicit=false);
    virtual void translateObjIDsToPtr();
    virtual void openPreview(){};
public slots:
    virtual void closePreview(){_inPreview=false;}
    virtual void paramModified(const QString& txt="")=0;
    virtual void KSlot(const SIG_TYPE&,void* data=nullptr);
signals:
    void selected(xVAbstractBaseObj*);
    void KSignal(const SIG_TYPE&,void* data=nullptr);
protected slots:
    void grpSelected()
    {
        emit selected(this);
    }
protected:
    bool _inPreview = false;
    bool _hasPreview = false;
    bool _selected = false;
    xVO_TYPE _type;
    QMap<QString,xPROP_TYPE> _paramMp;
    QString _id;
};

#endif // XVABSTRACTBASEOBJ_H
