#include "dialognew.h"
#include "ui_dialognew.h"

DialogNew::DialogNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNew)
{
    ui->setupUi(this);

    modelDozNew = new ModelDozNew(this);
    ui->tableViewCont->setModel(modelDozNew);
    ui->tableViewCont->verticalHeader()->setDefaultSectionSize(ui->tableViewCont->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewCont->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
    ui->lineEditMas->setValidator(new QDoubleValidator(0,1000000,1,this));
    ui->lineEditMas->setText(QString("750"));

    ui->tableViewRcp->verticalHeader()->setDefaultSectionSize(ui->tableViewRcp->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewRcp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewRcp->setModel(Rels::instance()->relRcp->proxyModel());
    Rels::instance()->relRcp->proxyModel()->setHeaderData(1,Qt::Horizontal,QString("Рецептура"));
    ui->tableViewRcp->setColumnHidden(0,true);
    for (int i=2; i<ui->tableViewRcp->model()->columnCount(); i++){
        ui->tableViewRcp->setColumnHidden(i,true);
    }
    ui->tableViewRcp->setColumnWidth(1,150);

    connect(ui->pushButtonCansel,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect(ui->pushButtonOk,SIGNAL(clicked(bool)),this,SLOT(writeRcp()));
    connect(ui->pushButtonSelect,SIGNAL(clicked(bool)),this,SLOT(selectRcp()));
}

DialogNew::~DialogNew()
{
    delete ui;
}

int DialogNew::getNewId()
{
    return modelDozNew->getNewId();
}

void DialogNew::selectRcp()
{
    QDateTime dt;
    dt.setDate(ui->dateEdit->date());
    dt.setTime(ui->timeEdit->time());
    QString smas=ui->lineEditMas->text().replace(",",".");
    double mas=smas.toDouble();
    QModelIndex indexRcp=ui->tableViewRcp->model()->index(ui->tableViewRcp->currentIndex().row(),0);
    QModelIndex indexNam=ui->tableViewRcp->model()->index(ui->tableViewRcp->currentIndex().row(),1);
    int id_rcp=ui->tableViewRcp->model()->data(indexRcp,Qt::EditRole).toInt();
    ui->labelRcp->setText(ui->tableViewRcp->model()->data(indexNam,Qt::DisplayRole).toString());

    modelDozNew->refresh(mas,dt,id_rcp);
    ui->tableViewCont->setColumnWidth(0,170);
    ui->tableViewCont->setColumnWidth(1,70);
    ui->tableViewCont->setColumnWidth(2,70);
    ui->tableViewCont->setColumnWidth(3,70);
    ui->tableViewCont->setColumnHidden(4,true);
    ui->tableViewCont->setColumnHidden(5,true);

    ui->pushButtonOk->setEnabled(true);
    ui->pushButtonSelect->setEnabled(false);
    ui->tableViewRcp->setEnabled(false);
    ui->lineEditMas->setEnabled(false);
    ui->dateEdit->setEnabled(false);
    ui->timeEdit->setEnabled(false);
}

void DialogNew::writeRcp()
{
    if (modelDozNew->writeRcp()){
        this->accept();
    }
}
