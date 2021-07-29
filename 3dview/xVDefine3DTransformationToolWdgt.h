#ifndef XVDEFINE3DTRANSFORMATIONTOOLWDGT_H
#define XVDEFINE3DTRANSFORMATIONTOOLWDGT_H

#include <QWidget>

namespace Ui {
class xVDefine3DTransformationToolWdgt;
}

class xVDefine3DTransformationToolWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit xVDefine3DTransformationToolWdgt(QWidget *parent = nullptr);
    ~xVDefine3DTransformationToolWdgt();

private:
    Ui::xVDefine3DTransformationToolWdgt *ui;
};

#endif // XVDEFINE3DTRANSFORMATIONTOOLWDGT_H
