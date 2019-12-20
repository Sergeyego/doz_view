#include "dialoggroup.h"
#include "ui_dialoggroup.h"

DialogGroup::DialogGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGroup)
{
    ui->setupUi(this);
    modelGroupEl = new ModelGroupEl(this);
    ui->tableView->setModel(modelGroupEl);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1,150);
    ui->tableView->setColumnWidth(2,100);
}

DialogGroup::~DialogGroup()
{
    delete ui;
}
