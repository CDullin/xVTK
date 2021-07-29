#include "xVDefine3DTransformationToolWdgt.h"
#include "ui_xVDefine3DTransformationToolWdgt.h"

xVDefine3DTransformationToolWdgt::xVDefine3DTransformationToolWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xVDefine3DTransformationToolWdgt)
{
    ui->setupUi(this);
}

xVDefine3DTransformationToolWdgt::~xVDefine3DTransformationToolWdgt()
{
    delete ui;
}
