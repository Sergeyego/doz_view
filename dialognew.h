#ifndef DIALOGNEW_H
#define DIALOGNEW_H

#include <QDialog>
#include "rels.h"
#include "modeldoz.h"

namespace Ui {
class DialogNew;
}

class DialogNew : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNew(QWidget *parent = 0);
    ~DialogNew();
    int getNewId();

private:
    Ui::DialogNew *ui;
    ModelDozNew *modelDozNew;

private slots:
    void selectRcp();
    void writeRcp();
};

#endif // DIALOGNEW_H
