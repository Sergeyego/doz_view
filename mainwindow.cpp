#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    groupSort = new QButtonGroup(this);
    groupSort->addButton(ui->radioButtonDate);
    groupSort->addButton(ui->radioButtonName);

    groupReport = new QButtonGroup(this);
    groupReport->addButton(ui->radioButtonComp);
    groupReport->addButton(ui->radioButtonPart);

    modelDoz = new ModelDoz(this);
    modelDozData = new ModelDozData(this);

    ui->tableViewDoz->setModel(modelDoz);
    ui->tableViewDoz->setColumnHidden(0,true);
    ui->tableViewDoz->setColumnWidth(1,70);
    ui->tableViewDoz->setColumnWidth(2,50);
    ui->tableViewDoz->setColumnWidth(3,150);
    ui->tableViewDoz->setColumnWidth(4,60);
    ui->tableViewDoz->setColumnWidth(5,60);
    ui->tableViewDoz->setColumnWidth(6,60);

    ui->tableViewData->setModel(modelDozData);
    ui->tableViewData->setColumnHidden(0,true);
    ui->tableViewData->setColumnWidth(1,170);
    ui->tableViewData->setColumnWidth(2,60);
    ui->tableViewData->setColumnWidth(3,60);
    ui->tableViewData->setColumnWidth(4,70);
    ui->tableViewData->setColumnWidth(5,70);
    ui->tableViewData->setColumnWidth(6,150);

    ui->actionUpdate->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->toolButtonUpd->setDefaultAction(ui->actionUpdate);

    ui->actionDel->setIcon(this->style()->standardIcon(QStyle::SP_TrashIcon));
    ui->toolButtonDel->setDefaultAction(ui->actionDel);

    ui->actionConfirm->setIcon(this->style()->standardIcon(QStyle::SP_DialogApplyButton));
    ui->toolButtonConfirm->setDefaultAction(ui->actionConfirm);

    ui->actionNew->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
    ui->toolButtonNew->setDefaultAction(ui->actionNew);

    ui->actionReport->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->toolButtonReport->setDefaultAction(ui->actionReport);

    connect(ui->actionUpdate,SIGNAL(triggered(bool)),this,SLOT(updDoz()));
    connect(ui->actionUpdate,SIGNAL(triggered(bool)),Rels::instance(),SLOT(refresh()));
    connect(ui->actionDel,SIGNAL(triggered(bool)),this,SLOT(delDoz()));
    connect(ui->actionConfirm,SIGNAL(triggered(bool)),this,SLOT(confirmDoz()));
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(newDoz()));
    connect(ui->actionReport,SIGNAL(triggered(bool)),this,SLOT(reportDoz()));

    connect(ui->tableViewDoz->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updDozData(QModelIndex)));

    updDoz();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", "doz_view");
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
    this->restoreState(settings.value("main_state").toByteArray());
    this->ui->splitter->restoreState(settings.value("main_splitter_width").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", "doz_view");
    settings.setValue("main_state", this->saveState());
    settings.setValue("main_geometry", this->saveGeometry());
    settings.setValue("main_splitter_width",ui->splitter->saveState());
}

void MainWindow::updDoz()
{
    modelDoz->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->radioButtonName->isChecked());
    toEndDoz();
}

void MainWindow::updDozData(QModelIndex index)
{
    QModelIndex indexId=ui->tableViewDoz->model()->index(index.row(),0);
    int id_doz=ui->tableViewDoz->model()->data(indexId,Qt::EditRole).toInt();
    modelDozData->refresh(id_doz);
}

void MainWindow::toEndDoz()
{
    if (ui->tableViewDoz->model()->rowCount()){
        ui->tableViewDoz->selectRow(ui->tableViewDoz->model()->rowCount()-1);
    }
}

void MainWindow::delDoz()
{
    modelDoz->removeRow(ui->tableViewDoz->currentIndex().row());
}

void MainWindow::confirmDoz()
{
    modelDoz->confirmDoz(ui->tableViewDoz->currentIndex().row());
    modelDozData->select();
}

void MainWindow::newDoz()
{
    DialogNew d;
    if (d.exec()==QDialog::Accepted){
        modelDoz->select();
        const int newId=d.getNewId();
        bool fok=false;
        for(int i=ui->tableViewDoz->model()->rowCount()-1; i>=0; i--){
            QModelIndex ind=ui->tableViewDoz->model()->index(i,0);
            if (ui->tableViewDoz->model()->data(ind,Qt::EditRole).toInt()==newId){
                ui->tableViewDoz->selectRow(i);
                fok=true;
            }
        }
        if (!fok){
            toEndDoz();
        }
    }
}

void MainWindow::reportDoz()
{
    DialogReport d(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->radioButtonPart->isChecked());
    d.exec();
}
