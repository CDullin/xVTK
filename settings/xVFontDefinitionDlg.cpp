#include "xVFontDefinitionDlg.h"
#include "ui_xVFontDefinitionDlg.h"

#include <QPainter>

xVFontDefinitionDlg::xVFontDefinitionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xVFontDefinitionDlg)
{
    ui->setupUi(this);
    connect(ui->pExampleTxtLEdit,SIGNAL(textChanged(const QString &)),this,SLOT(displayFont()));
    connect(ui->pPropTable,SIGNAL(paramModified(const QString&)),this,SLOT(displayFont()));
}

xVFontDefinitionDlg::~xVFontDefinitionDlg()
{
    delete ui;
}

int xVFontDefinitionDlg::exec()
{
    show();
    displayFont();
    return QDialog::exec();
}

void xVFontDefinitionDlg::displayFont()
{
    // calculate pixmap
    QPixmap pix(ui->pExampleLab->contentsRect().width(),ui->pExampleLab->contentsRect().height());
    QPainter pain(&pix);
    pain.setRenderHints(QPainter::TextAntialiasing|QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing);
    QBrush br(QPixmap("://images/opacity_pattern.png"));
    pain.setBrush(br);
    pain.drawRect(0,0,pix.width(),pix.height());
    pain.setBrush(QBrush(QColor(0,0,0,128)));
    pain.drawRect(0,0,pix.width(),pix.height());

    if (pParamMpRef)
    {

        QTransform t;
        t.translate(pix.width()/2,pix.height()/2);
        t.rotate((*pParamMpRef)["orientation [°]"]._value.value<xLimitedInt>()._value);
        pain.setTransform(t);

        QFont f;
        f.setFamily((*pParamMpRef)["font family"]._value.toString());
        f.setBold((*pParamMpRef)["bold"]._value.toBool());
        f.setItalic((*pParamMpRef)["italic"]._value.toBool());
        f.setPixelSize((*pParamMpRef)["size"]._value.value<xLimitedInt>()._value);
        pain.setFont(f);

        int _alignment = Qt::AlignHCenter|Qt::AlignVCenter;
        switch (_optionLsts["horizontal alignment"].indexOf((*pParamMpRef)["horizontal alignment"]._value.toString()))
        {
        case 0 : _alignment = Qt::AlignLeft;break;
        case 1 : _alignment = Qt::AlignHCenter;break;
        case 2 : _alignment = Qt::AlignRight;break;
        }
        switch (_optionLsts["vertical alignment"].indexOf((*pParamMpRef)["vertical alignment"]._value.toString()))
        {
        case 0 : _alignment |= Qt::AlignTop;break;
        case 1 : _alignment |= Qt::AlignVCenter;break;
        case 2 : _alignment |= Qt::AlignBottom;break;
        }

        QRect _boundingRect(-pix.width()/2,-pix.height()/2,pix.width(),pix.height());
        if ((*pParamMpRef)["tight bounding box"]._value.toBool())
        {
            _boundingRect = pain.boundingRect(_boundingRect,_alignment,ui->pExampleTxtLEdit->text());
        }

        pain.setPen(Qt::NoPen);
        QColor backgroundColor((*pParamMpRef)["background color"]._value.value<QColor>());
        backgroundColor.setAlphaF((*pParamMpRef)["background opacity"]._value.value<xLimitedDouble>()._value);
        pain.setBrush(QBrush(backgroundColor));
        pain.drawRect(_boundingRect);

        if ((*pParamMpRef)["shadow"]._value.toBool())
        {
            pain.setPen((*pParamMpRef)["shadow color"]._value.value<QColor>());
            QPoint offset = (*pParamMpRef)["shadow offset [pnt]"]._value.value<QPoint>();
            pain.drawText(_boundingRect.translated(offset.x(),offset.y()),_alignment,ui->pExampleTxtLEdit->text());
        }

        QColor color((*pParamMpRef)["color"]._value.value<QColor>());
        color.setAlphaF((*pParamMpRef)["opacity"]._value.value<xLimitedDouble>()._value);
        pain.setPen(color);
        pain.drawText(_boundingRect,_alignment,ui->pExampleTxtLEdit->text());

        if ((*pParamMpRef)["frame visible"]._value.toBool())
        {
            pain.setBrush(Qt::NoBrush);
            QPen pen((*pParamMpRef)["frame color"]._value.value<QColor>(),(*pParamMpRef)["frame width"]._value.value<xLimitedInt>()._value);
            pain.setPen(pen);
            pain.drawRect(_boundingRect );
        }
        pain.resetTransform();
    }
    else
    {
        pain.drawText(0,0,pix.width(),pix.height(),Qt::AlignCenter,ui->pExampleTxtLEdit->text());
    }
    pain.end();
    ui->pExampleLab->setPixmap(pix);
}

void xVFontDefinitionDlg::setParameterRef(xParamMap *pRef)
{
    pParamMpRef = pRef;
    pCopyParamMp = new xParamMap(*pRef);
    // fill in gui
    ui->pPropTable->updateTable(pParamMpRef);
    displayFont();
}

void xVFontDefinitionDlg::accept()
{
    // copy values from gui to parameter map
    QDialog::accept();
}

void xVFontDefinitionDlg::reject()
{
    pParamMpRef = pCopyParamMp;
    QDialog::reject();
}

void xVFontDefinitionDlg::KSlot(const SIG_TYPE& _type,void *pData)
{

}
