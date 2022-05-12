#include "dialogbunk.h"
#include "ui_dialogbunk.h"

DialogBunk::DialogBunk(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBunk)
{
    ui->setupUi(this);
    ui->dateTimeEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    ui->pushButtonUpdLoad->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonCurrentTime->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->pushButtonUpdCurrent->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonPart->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));

    modelCurrentBunk = new ModelCurrentBunk(this);

    modelBunk = new QSortFilterProxyModel(this);
    modelBunk->setSourceModel(modelCurrentBunk);
    modelBunk->setFilterKeyColumn(5);
    modelBunk->setFilterFixedString("0");

    modelPart = new QSortFilterProxyModel(this);
    modelPart->setSourceModel(modelCurrentBunk);
    modelPart->setFilterKeyColumn(5);
    modelPart->setFilterFixedString("1");

    ui->tableViewCurrent->setModel(modelBunk);
    ui->tableViewComp->setModel(modelPart);
    updCurrentTime();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelLoadBunk = new ModelLoadBunk(this);
    ui->tableViewLoad->setModel(modelLoadBunk);
    updLoad();
    ui->tableViewLoad->setColumnHidden(0,true);
    ui->tableViewLoad->setColumnWidth(1,90);
    ui->tableViewLoad->setColumnWidth(2,60);
    ui->tableViewLoad->setColumnWidth(3,80);
    ui->tableViewLoad->setColumnWidth(4,190);
    ui->tableViewLoad->setColumnWidth(5,70);
    ui->tableViewLoad->setColumnWidth(6,120);
    ui->tableViewLoad->setColumnWidth(7,100);

    connect(ui->pushButtonCurrentTime,SIGNAL(clicked(bool)),this,SLOT(updCurrentTime()));
    connect(ui->pushButtonUpdCurrent,SIGNAL(clicked(bool)),this,SLOT(updCurrent()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->pushButtonUpdLoad,SIGNAL(clicked(bool)),this,SLOT(updLoad()));
    connect(ui->pushButtonPart,SIGNAL(clicked(bool)),this,SLOT(calcPart()));
    connect(modelLoadBunk,SIGNAL(sigUpd()),this,SLOT(updCurrent()));
}

DialogBunk::~DialogBunk()
{
    delete ui;
}

void DialogBunk::updCurrent()
{
    modelCurrentBunk->refresh(ui->dateTimeEdit->dateTime());
    ui->tableViewCurrent->setColumnHidden(5,true);
    ui->tableViewComp->setColumnHidden(0,true);
    ui->tableViewComp->setColumnHidden(5,true);
    ui->tableViewCurrent->resizeToContents();
    ui->tableViewComp->resizeToContents();
}

void DialogBunk::updCurrentTime()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    updCurrent();
}

void DialogBunk::save()
{
    TableView v;
    v.setModel(modelCurrentBunk);
    v.setColumnHidden(5,true);
    v.verticalHeader()->setVisible(false);
    v.resizeToContents();
    v.save(QString::fromUtf8("Состояние бункеров на ")+ui->dateTimeEdit->dateTime().toString("dd.MM.yy HH.mm"));
}

void DialogBunk::updLoad()
{
    modelLoadBunk->refresh(ui->dateEditBeg->date(), ui->dateEditEnd->date());
}

void DialogBunk::calcPart()
{
    if (ui->dateEditEnd->date().addDays(-3)>=ui->dateEditBeg->date()){
        QMessageBox::critical(this,QString::fromUtf8("Недопустимое действие"),QString::fromUtf8("Запрещено пересчитывать партии более чем за 3 дня"),QMessageBox::Cancel);
        return;
    }
    QString text=QString::fromUtf8("Будут обновлены партии на дозировках с %1 по %2.").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));
    int q = QMessageBox::question(this,QString::fromUtf8("Подтвердите действие"),text,QMessageBox::Ok,QMessageBox::Cancel);
    if (q==QMessageBox::Ok){
        modelLoadBunk->updatePart(ui->dateEditBeg->date(), ui->dateEditEnd->date());
        emit sigPart();
    }
}
