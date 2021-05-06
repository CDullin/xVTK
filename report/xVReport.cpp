#include "xVReport.h"
#include <QSaveFile>
#include <QTextStream>
#include <QVector3D>
#include "xvEvalCondition.h"
#include "xVObjectTypes.h"
#include <QTextBlock>
#include <QTextDocument>
#include <QDir>

xVReport::xVReport(const QString& name, QWidget* parent):QTextEdit(parent),_name(name)
{
    _lastModified = QDateTime::currentDateTime();
    _lastAutoSaved = _lastModified;
    QTextEdit::append(QString("Report %1 created ").arg(_name)+QDateTime::currentDateTime().toString("hh:mm:ss dd-MMM-yyyy"));
    QTextEdit::append(QString("\n"));
    connect(this,SIGNAL(textChanged()),this,SLOT(setModified()));
}

void xVReport::setModified()
{
    _modified = true;
}

void xVReport::load(QString fname)
{
    QFileInfo info(fname);

    QString _currentPath = QDir::currentPath();
    QDir::setCurrent(info.absolutePath());

    QFile f(fname);
    if (f.open(QFile::ReadOnly)) {
        QTextStream t(&f);
        QString s;
        s=t.readAll(); setHtml(s);
        _modified = false;
        _lastModified = QDateTime::currentDateTime();
        _lastAutoSaved = _lastModified;
        _fileName = fname;
        _boundToFile = true;
        _name = info.baseName();
        f.close();
    }
    else
        emit KSignal(ST_ERROR_MSG,new QString("Could not open file"));

    QDir::setCurrent(_currentPath);
}

void xVReport::autoSave()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString fname = ::_settings["temp folder"]._value.value<xFileName>()._fileName+"/"+::_session+"/reports"+"/"+_name;
    QDir d(fname);
    if (d.mkpath(fname))
    {
        // remove all old tmp files
        QFileInfoList lst=d.entryInfoList(QDir::Files);
        while (lst.count())
        {
            QFileInfo info = lst.takeLast();
            QFile::remove(info.absoluteFilePath());
        }
        fname+="/"+_name+QString("_")+dt.toString("yyMMddhhmmsszzz");
        _lastAutoSaved = dt;

        QFileInfo info(fname);
        QFile f(fname);
        if (f.open(QFile::WriteOnly | QFile::Append)) {
            QTextStream t(&f);
            t << toHtml();
            f.close();

            // now we save all images
            QStringList images;
            QTextBlock b = document()->begin();
            while (b.isValid()) {
                for (QTextBlock::iterator i = b.begin(); !i.atEnd(); ++i) {
                    QTextCharFormat format = i.fragment().charFormat();
                    if (format.isImageFormat())
                        images << format.toImageFormat().name();
                }
                b = b.next();
            }

            for (int i=0;i<images.count();++i) {
                QVariant res=document()->resource(QVariant::Image,QUrl(images[i]));
                res.value<QImage>().save(info.absolutePath()+"/"+images[i]);
            }

            emit KSignal(ST_MSG,new QString(QString("Report %1 successfully auto saved").arg(_name)));
        }
    }
    else
        emit KSignal(ST_ERROR_MSG,new QString("Couldn't create tmp dir"));
}

void xVReport::save(QString fname)
{
    QFileInfo info(fname);
    QFile f(fname);
    if (f.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream t(&f);
        t << toHtml();
        _modified = false;
        _lastModified = QDateTime::currentDateTime();
        _fileName = fname;
        _boundToFile = true;
        _name = info.baseName();
        f.close();

        // now we save all images
        QStringList images;
        QTextBlock b = document()->begin();
        while (b.isValid()) {
            for (QTextBlock::iterator i = b.begin(); !i.atEnd(); ++i) {
                QTextCharFormat format = i.fragment().charFormat();
                if (format.isImageFormat())
                    images << format.toImageFormat().name();
            }
            b = b.next();
        }

        for (int i=0;i<images.count();++i) {
            QVariant res=document()->resource(QVariant::Image,QUrl(images[i]));
            res.value<QImage>().save(info.absolutePath()+"/"+images[i]);
        }
    }
    else
        emit KSignal(ST_ERROR_MSG,new QString("Could not open file"));
}

void xVReport::append(const xVReport_TRANSFER& trans)
{
    QString s(trans._name+"\t");
    switch (trans._prop._value.type())
    {
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::UInt:
    case QVariant::ULongLong:
        s+=QString("%1").arg(trans._prop._value.toLongLong());
        break;
    case QVariant::Double:
        s+=QString("%1").arg(trans._prop._value.toDouble());
        break;
    case QVariant::Bool:
        s+=trans._prop._value.toBool() ? QString("true") : QString("false");
        break;
    case QVariant::Color:
    {
        QColor col=trans._prop._value.value<QColor>();
        s+=QString("(%1, %2, %3, %4)").arg(col.red()).arg(col.green()).arg(col.blue()).arg(col.alpha());
    }
        break;
    case QVariant::Vector3D:
    {
        QVector3D v=trans._prop._value.value<QVector3D>();
        s+=QString("(%1, %2, %3)").arg(v.x()).arg(v.y()).arg(v.z());
    }
        break;
    case QVariant::String:
        s+=trans._prop._value.toString();
        break;
    case QVariant::UserType:
    {
        if (trans._prop._value.userType()==QMetaType::type("QFileInfo"))
            s+=trans._prop._value.value<QFileInfo>().absoluteFilePath();
        if (trans._prop._value.userType()==QMetaType::type("x3D_SAMPLE_POS"))
            s+=QString("(x=%1, y=%2, z=%3)")
                    .arg(trans._prop._value.value<x3D_SAMPLE_POS>().x)
                    .arg(trans._prop._value.value<x3D_SAMPLE_POS>().y)
                    .arg(trans._prop._value.value<x3D_SAMPLE_POS>().z);
        if (trans._prop._value.userType()==QMetaType::type("vtkPiecewiseFunctionPtr"))
        {
            // direct update via ptr
            s+="not supported yet";
        }
        if (trans._prop._value.userType()==QMetaType::type("vtkColorTransferFunctionPtr"))
        {
            s+="not supported yet";
        }
        if (trans._prop._value.userType()==QMetaType::type("xLimitedInt"))
        {
            s+=QString("%1 (%2 .. %3)")
                    .arg(trans._prop._value.value<xLimitedInt>()._value)
                    .arg(trans._prop._value.value<xLimitedInt>()._lowerLimit)
                    .arg(trans._prop._value.value<xLimitedInt>()._upperLimit);
        }
        if (trans._prop._value.userType()==QMetaType::type("xLimitedDouble"))
        {
            s+=QString("%1 (%2 .. %3)")
                    .arg(trans._prop._value.value<xLimitedDouble>()._value)
                    .arg(trans._prop._value.value<xLimitedDouble>()._lowerLimit)
                    .arg(trans._prop._value.value<xLimitedDouble>()._upperLimit);
        }
        if (trans._prop._value.userType()==QMetaType::type(("xParamMap")))
        {
            s+="not supported yet";
        }
        if (trans._prop._value.userType()==QMetaType::type(("xVEvalCondition")))
            s+=trans._prop._value.value<xVEvalCondition>().equation();
        if (trans._prop._value.userType()==QMetaType::type(("vtkImageDataPtr")))
        {
            QImage image=xVImageObj::convertToQImage(trans._prop._value.value<vtkImageDataPtr>());

            QTextCursor cursor = this->textCursor();
            QTextDocument *document = this->document();
            QString ID = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
            document->addResource(QTextDocument::ImageResource, QUrl(QString("image_%1_%2.png").arg(ID).arg(_imageNr)), image);
            cursor.insertImage(QString("image_%1_%2.png").arg(ID).arg(_imageNr));
            ++_imageNr;
        }
    }
        break;
    default:
        QTextEdit::append("unhandled datatype");
        break;
    }
    QTextEdit::append(s);
}

