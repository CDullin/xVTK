#ifndef XVVISMAINWIN_H
#define XVVISMAINWIN_H

#include <QMainWindow>

namespace Ui {
class xVVisMainWin;
}

class xVVisMainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit xVVisMainWin(QWidget *parent = nullptr);
    ~xVVisMainWin();

    void addWindowAction(QAction*);
    void removeWindowAction(QWidget* wdgt);

private:
    Ui::xVVisMainWin *ui;
};

#endif // XVVISMAINWIN_H
