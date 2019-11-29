#ifndef DIALOGBUNK_H
#define DIALOGBUNK_H

#include <QDialog>
#include "modeldoz.h"

namespace Ui {
class DialogBunk;
}

class DialogBunk : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBunk(QWidget *parent = 0);
    ~DialogBunk();

private:
    Ui::DialogBunk *ui;
    ModelCurrentBunk *modelCurrentBunk;
    ModelCurrentPart *modelCurrentPart;
    ModelLoadBunk *modelLoadBunk;

private slots:
    void updCurrent();
    void updCurrentTime();
    void save();
    void updLoad();
    void calcPart();

signals:
    void sigPart();
};

#endif // DIALOGBUNK_H
