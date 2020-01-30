#ifndef DIALOGBUNK_H
#define DIALOGBUNK_H

#include <QDialog>
#include <QSortFilterProxyModel>
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
    ModelLoadBunk *modelLoadBunk;
    QSortFilterProxyModel *modelBunk;
    QSortFilterProxyModel *modelPart;

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
