#ifndef XVIMAGEOBJ_H
#define XVIMAGEOBJ_H

#include "xVTypes.h"
#include "xVObjects.h"
#include "xVGenImpObj.h"

class xVImageObj:public xVGenImpObj
{
    Q_OBJECT
public:
    xVImageObj(const QString& txt="");
    xVImageObj(QDataStream&);
    virtual void run() override;
    virtual void reset() override;

    static QImage convertToQImage(vtkImageDataPtr vtkImage);
    static vtkImageDataPtr tryDCMImport(const QString& fname);

};

#endif // XVIMAGEOBJ_H
