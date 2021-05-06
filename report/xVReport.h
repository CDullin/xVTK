#ifndef XVREPORT_H
#define XVREPORT_H

#include "xVTypes.h"
#include <QTextEdit>

class xVReport:public QTextEdit
{
    Q_OBJECT
public:
    QString _name;
    QString _fileName;
    bool _boundToFile = false;
    QDateTime _lastModified;
    bool _modified = false;
    QDateTime _lastAutoSaved;

    struct xVReport_TRANSFER
    {
    public:
        QString _name;
        xPROP_TYPE _prop;
        xVReport_TRANSFER(const QString& txt,xPROP_TYPE p){_name=txt,_prop=p;}
    };
    xVReport(const QString&,QWidget* parent=nullptr);
    void append(const xVReport_TRANSFER&);
    void save(QString fname);
    void load(QString fname);
    void autoSave();
signals:
    void KSignal(const SIG_TYPE&,void* data=nullptr);
protected slots:
    void setModified();
protected:
    long _imageNr=0;
};

#endif // XVREPORT_H
