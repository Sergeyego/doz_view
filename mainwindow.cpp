#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();

    cubeDoz = new CubeWidget(12);

    dialogReport = new DialogReport(this);

    dialogBunk = new DialogBunk(this);

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
    ui->tableViewData->setColumnHidden(7,true);

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

    ui->actionLoad->setIcon(this->style()->standardIcon(QStyle::SP_CommandLink));
    ui->actionHist->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogInfoView));
    ui->actionGrp->setIcon(this->style()->standardIcon(QStyle::SP_DialogHelpButton));

    ui->actionCube->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));

    ui->actionRstPart->setIcon(this->style()->standardIcon(QStyle::SP_DialogResetButton));

    ui->actionSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    ui->toolButtonSave->setDefaultAction(ui->actionSave);

    connect(ui->actionUpdate,SIGNAL(triggered(bool)),this,SLOT(updDoz()));
    connect(ui->actionUpdate,SIGNAL(triggered(bool)),Rels::instance(),SLOT(refresh()));
    connect(ui->actionDel,SIGNAL(triggered(bool)),this,SLOT(delDoz()));
    connect(ui->actionConfirm,SIGNAL(triggered(bool)),this,SLOT(confirmDoz()));
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(newDoz()));
    connect(ui->actionReport,SIGNAL(triggered(bool)),this,SLOT(reportDoz()));
    connect(ui->actionLoad,SIGNAL(triggered(bool)),dialogBunk,SLOT(show()));
    connect(ui->actionCube,SIGNAL(triggered(bool)),cubeDoz,SLOT(show()));
    connect(ui->actionRstPart,SIGNAL(triggered(bool)),this,SLOT(resetEdtPart()));
    connect(ui->actionHist,SIGNAL(triggered(bool)),this,SLOT(histLoad()));
    connect(ui->actionGrp,SIGNAL(triggered(bool)),this,SLOT(groupEl()));
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(saveFile()));

    connect(ui->tableViewDoz->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updDozData(QModelIndex)));
    connect(dialogBunk,SIGNAL(sigPart()),modelDozData,SLOT(select()));

    updDoz();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
    delete cubeDoz;
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
    this->restoreState(settings.value("main_state").toByteArray());
    this->ui->splitter->restoreState(settings.value("main_splitter_width").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
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
    dialogReport->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),ui->radioButtonPart->isChecked());
    dialogReport->show();
}

void MainWindow::resetEdtPart()
{
    int row=ui->tableViewData->currentIndex().row();
    QModelIndex ind=ui->tableViewData->model()->index(row,7);
    if (ind.isValid()){
        ui->tableViewData->model()->setData(ind,false,Qt::EditRole);
        modelDozData->submit();
    }
}

void MainWindow::histLoad()
{
    DialogHist h;
    h.exec();
}

void MainWindow::groupEl()
{
    DialogGroup g;
    g.exec();
}

void MainWindow::saveFile()
{
    int row=ui->tableViewDoz->currentIndex().row();
    QString rcp=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,3),Qt::DisplayRole).toString();
    QString dat=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,1),Qt::EditRole).toDate().toString("dd.MM.yy");
    QString tim=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,2),Qt::EditRole).toTime().toString("hh:mm");
    QString kvo=QString::number(ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,4),Qt::EditRole).toDouble());
    DbSave s(ui->tableViewData);
    s.save(rcp+" "+dat+" "+tim+" "+kvo+QString::fromUtf8(" кг"));
}

