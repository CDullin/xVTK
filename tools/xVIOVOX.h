#ifndef XVIOVOX_H
#define XVIOVOX_H

#include "xVTypes.h"
#include "xVGenVolIO.h"
#include <QObject>

class xVIOVOX:public xVGenVolIO
{
    Q_OBJECT
public:
    xVIOVOX(QObject* parent=nullptr):xVGenVolIO(parent){}
    virtual vtkImageDataPtr read(const QString& fname) override;
    virtual void write(vtkImageDataPtr data,const QString& fname) override;
};

#endif // XVIOVOX_H
