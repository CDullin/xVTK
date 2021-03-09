#ifndef XVSETTINGSWDGT_H
#define XVSETTINGSWDGT_H

#include <QWidget>
#include "xVTypes.h"

namespace Ui {
class xVSettingsWdgt;
}

class xVSettingsWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit xVSettingsWdgt(QWidget *parent = nullptr);
    ~xVSettingsWdgt();

protected:
    void generateDefault();

private:
    Ui::xVSettingsWdgt *ui;
};

#endif // XVSETTINGSWDGT_H
