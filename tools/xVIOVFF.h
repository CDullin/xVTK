#ifndef XVIOVFF_H
#define XVIOVFF_H

#include "xVTypes.h"
#include "xVGenVolIO.h"
#include <QObject>

class xVIOVFF:public xVGenVolIO
{
    Q_OBJECT
public:
    xVIOVFF(QObject* parent=nullptr):xVGenVolIO(parent){}
    virtual vtkImageDataPtr read(const QString& fname) override;
    virtual void write(vtkImageDataPtr data,const QString& fname) override;
};

#endif // XVIOVFF_H
