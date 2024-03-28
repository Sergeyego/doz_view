#include "dialoghist.h"
#include "ui_dialoghist.h"

DialogHist::DialogHist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHist)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setDate(QDate::currentDate().addDays(-365));
    ui->dateTimeEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    ui->comboBoxCex->setModel(Rels::instance()->relCex->model());
    ui->comboBoxCex->setModelColumn(1);

    modelHist = new ModelHist(this);
    ui->tableViewHist->setModel(modelHist);

    ui->tableViewMatr->setModel(Rels::instance()->relComp->model());
    ui->tableViewMatr->model()->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Компонент"));
    ui->tableViewMatr->setColumnHidden(0,true);
    ui->tableViewMatr->resizeToContents();

    ui->splitter->setStretchFactor(0,3);
    ui->splitter->setStretchFactor(1,5);

    connect(ui->tableViewMatr->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updHist()));
    connect(ui->dateTimeEdit,SIGNAL(dateTimeChanged(QDateTime)),this,SLOT(updHist()));
    connect(modelHist,SIGNAL(sigRefresh()),ui->tableViewHist,SLOT(resizeToContents()));
    connect(ui->comboBoxCex,SIGNAL(currentIndexChanged(int)),this,SLOT(updHist()));
}

DialogHist::~DialogHist()
{
    delete ui;
}

void DialogHist::updHist()
{
    QModelIndex indMatr=ui->tableViewMatr->model()->index(ui->tableViewMatr->currentIndex().row(),0);
    int id_matr=ui->tableViewMatr->model()->data(indMatr,Qt::EditRole).toInt();
    int id_cex=ui->comboBoxCex->model()->data(ui->comboBoxCex->model()->index(ui->comboBoxCex->currentIndex(),0),Qt::EditRole).toInt();
    modelHist->refresh(id_matr,ui->dateTimeEdit->dateTime(),id_cex);
}
