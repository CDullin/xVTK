#ifndef XVICONFACTORY_H
#define XVICONFACTORY_H

#include <QIcon>
#include <QMap>
#include <QPixmap>

class xVIconFactory
{
public:
    xVIconFactory();
    bool contains(const QString& pattern){return _iconMp.contains(pattern);}
    QPixmap pixmap(const QString& pattern){return _iconMp[pattern];}
    QIcon icon(const QString& pattern){return QIcon(pixmap(pattern));}
protected:
    void populateMap();
    QMap <QString,QPixmap> _iconMp;
};

extern xVIconFactory _iconFactory;

#endif // XVICONFACTORY_H
