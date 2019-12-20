#ifndef DIALOGGROUP_H
#define DIALOGGROUP_H

#include <QDialog>
#include "modeldoz.h"

namespace Ui {
class DialogGroup;
}

class DialogGroup : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGroup(QWidget *parent = 0);
    ~DialogGroup();

private:
    Ui::DialogGroup *ui;
    ModelGroupEl *modelGroupEl;
};

#endif // DIALOGGROUP_H
