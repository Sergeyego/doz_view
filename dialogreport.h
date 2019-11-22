#ifndef DIALOGREPORT_H
#define DIALOGREPORT_H

#include <QDialog>
#include <QDate>
#include "modeldoz.h"

namespace Ui {
class DialogReport;
}

class DialogReport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogReport(QDate begDate, QDate endDate, bool byPart, QWidget *parent = 0);
    ~DialogReport();

private:
    Ui::DialogReport *ui;
    QString header;
    ModelReport *modelReport;

private slots:
    void save();
};

#endif // DIALOGREPORT_H
