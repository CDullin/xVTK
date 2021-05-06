#ifndef XVIOVOL_H
#define XVIOVOL_H

#include "xVTypes.h"
#include "xVGenVolIO.h"
#include <QObject>

class xVIOVOL:public xVGenVolIO
{
    Q_OBJECT
public:
    xVIOVOL(QObject* parent=nullptr):xVGenVolIO(parent){}
    virtual vtkImageDataPtr read(const QString& fname) override;
    virtual void write(vtkImageDataPtr data,const QString& fname) override;
};

#endif // XVIOVOL_H
