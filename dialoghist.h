#ifndef DIALOGHIST_H
#define DIALOGHIST_H

#include <QDialog>
#include <QList>
#include "rels.h"
#include "modeldoz.h"
#include <QCalendarWidget>

namespace Ui {
class DialogHist;
}

class DialogHist : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHist(QWidget *parent = 0);
    ~DialogHist();

private:
    Ui::DialogHist *ui;
    ModelHist *modelHist;

private slots:
    void updHist();
};

#endif // DIALOGHIST_H
