#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();

    QStringList axes;
    axes.push_back(tr("Рецептура"));
    axes.push_back(tr("Компонент"));
    axes.push_back(tr("Партия дозировки"));
    axes.push_back(tr("Партия электродов"));
    axes.push_back(tr("Марка"));
    axes.push_back(tr("Диаметр"));
    axes.push_back(tr("Группа"));
    axes.push_back(tr("Год"));
    axes.push_back(tr("Месяц"));
    axes.push_back(tr("День"));
    QString query="select n.nam, mt.nam, d.parti, p.n_s||'-'||date_part('year',p.dat_part), e.marka, p.diam, t.nam, "
            "substr(cast(d.dat as char(32)),1,4) as yr, "
            "substr(cast(d.dat as char(32)),1,7) as mn, "
            "substr(cast(d.dat as char(32)),1,10) as dy, "
            "ds.kvo_fact*m.kvo/d.kvo_tot as kvo "
            "from parti_mix as m "
            "inner join dosage as d on d.id=m.id_dos "
            "inner join dosage_spnd as ds on ds.id_dos=d.id "
            "inner join matr as mt on mt.id=ds.id_comp "
            "inner join parti as p on p.id=m.id_part "
            "inner join elrtr as e on e.id=p.id_el "
            "inner join el_types as t on t.id=e.id_vid "
            "inner join rcp_nam as n on n.id=d.id_rcp "
            "where d.dat between :d1 and :d2";
    cubeDoz = new CubeWidget(QString("Расход компонентов на партии электродов, кг"),axes,query,2);

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

    ui->actionCube->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));

    connect(ui->actionUpdate,SIGNAL(triggered(bool)),this,SLOT(updDoz()));
    connect(ui->actionUpdate,SIGNAL(triggered(bool)),Rels::instance(),SLOT(refresh()));
    connect(ui->actionDel,SIGNAL(triggered(bool)),this,SLOT(delDoz()));
    connect(ui->actionConfirm,SIGNAL(triggered(bool)),this,SLOT(confirmDoz()));
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(newDoz()));
    connect(ui->actionReport,SIGNAL(triggered(bool)),this,SLOT(reportDoz()));
    connect(ui->actionLoad,SIGNAL(triggered(bool)),dialogBunk,SLOT(show()));
    connect(ui->actionCube,SIGNAL(triggered(bool)),cubeDoz,SLOT(show()));

    connect(ui->tableViewDoz->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updDozData(QModelIndex)));
    connect(dialogBunk,SIGNAL(sigPart()),modelDozData,SLOT(select()));

    updDoz();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
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

