#include "xVFileImportDlgItem.h"
#include <QFileDialog>
#include <QResizeEvent>

xVFileImportDlgItem::xVFileImportDlgItem(QWidget *parent):QWidget(parent)
{
    setGeometry(0,0,100,20);
    pLEdit = new QLineEdit(this);
    pLEdit->setGeometry(0,0,80,20);
    pLEdit->setAlignment(Qt::AlignRight);
    pBrowseTB = new QToolButton(this);
    pBrowseTB->setGeometry(80,0,20,20);
    pBrowseTB->setText("...");
    pLEdit->setReadOnly(true);
    connect(pBrowseTB,SIGNAL(clicked()),this,SLOT(browse()));
}

void xVFileImportDlgItem::setFilePath(const QString& txt)
{
    _currentInfo = QFileInfo(txt);
    pLEdit->setText(txt);
}

QFileInfo xVFileImportDlgItem::fileInfo()
{
    return _currentInfo;
}
void xVFileImportDlgItem::browse()
{
    QString s;
    switch (_mode)
    {
    case READ_FILE: s = QFileDialog::getOpenFileName(0,"chose file",_currentInfo.absoluteFilePath());break;
    case SAVE_FILE: s = QFileDialog::getSaveFileName(0,"chose file",_currentInfo.absoluteFilePath());break;
    case OPEN_DIR: s = QFileDialog::getExistingDirectory(0,"chose directory",_currentInfo.absoluteFilePath());break;
    }
    _currentInfo = QFileInfo(s);
    pLEdit->setText(s);
    emit modified();
}

void xVFileImportDlgItem::resizeEvent(QResizeEvent *event)
{
    QSize size=event->size();
    setGeometry(0,0,size.width(),size.height());
    pLEdit->setGeometry(0,0,size.width()-20,size.height());
    pBrowseTB->setGeometry(size.width()-20,0,20,size.height());
}
