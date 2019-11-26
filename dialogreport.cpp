#include "dialogreport.h"
#include "ui_dialogreport.h"

DialogReport::DialogReport(QDate begDate, QDate endDate, bool byPart, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogReport)
{
    ui->setupUi(this);
    ui->pushButtonSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    header=QString("Расход компонентов на линии дозировки с %1 по %2 ").arg(begDate.toString("dd.MM.yy")).arg(endDate.toString("dd.MM.yy"));
    header+=byPart? QString("по партиям") : QString("без партий");
    ui->label->setText(header);

    modelReport = new ModelReport(this);
    modelReport->refresh(begDate,endDate,byPart);
    ui->tableView->setModel(modelReport);
    ui->tableView->resizeToContents();

    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

DialogReport::~DialogReport()
{
    delete ui;
}

void DialogReport::save()
{
    ui->tableView->save(header);
}
