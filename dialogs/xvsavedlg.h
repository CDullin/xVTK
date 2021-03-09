#ifndef XVSAVEDLG_H
#define XVSAVEDLG_H

#include <QDialog>
#include <QPixmap>

namespace Ui {
class xVSaveDlg;
}

class xVSaveDlg : public QDialog
{
    Q_OBJECT

public:
    explicit xVSaveDlg(QWidget *parent = nullptr);
    ~xVSaveDlg();
    void setPixmap(const QPixmap& pix);
    virtual void accept() override;
    QString path(){return _path;}
    QStringList header();
    bool saveExplicit();

protected slots:
    void browseFolder();

private:
    QString _path="";
    Ui::xVSaveDlg *ui;
};

#endif // XVSAVEDLG_H
