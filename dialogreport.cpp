#include "dialogreport.h"
#include "ui_dialogreport.h"

DialogReport::DialogReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogReport)
{
    ui->setupUi(this);
    ui->pushButtonSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));

    modelReport = new ModelReport(this);
    ui->tableView->setModel(modelReport);

    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
}

DialogReport::~DialogReport()
{
    delete ui;
}

void DialogReport::refresh(QDate begDate, QDate endDate, bool byPart)
{
    header=QString::fromUtf8("Расход компонентов на линии дозировки с %1 по %2 ").arg(begDate.toString("dd.MM.yy")).arg(endDate.toString("dd.MM.yy"));
    header+=byPart? QString::fromUtf8("по партиям") : QString::fromUtf8("без партий");
    ui->label->setText(header);
    modelReport->refresh(begDate,endDate,byPart);
    ui->tableView->resizeToContents();
}

void DialogReport::save()
{
    ui->tableView->save(header);
}
