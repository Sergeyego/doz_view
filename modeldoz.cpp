#include "modeldoz.h"

ModelDoz::ModelDoz(QObject *parent) : DbTableModel("dosage",parent)
{
    addColumn("id",QString::fromUtf8("id"));
    addColumn("dat",QString::fromUtf8("Дата"));
    addColumn("tm",QString::fromUtf8("Время"));
    addColumn("id_rcp",QString::fromUtf8("Рецептура"),NULL,Rels::instance()->relRcp);
    addColumn("kvo_tot",QString::fromUtf8("К-во, кг"),new QDoubleValidator(0,1000000,1,this));
    addColumn("result",QString::fromUtf8("OK"));
    addColumn("parti",QString::fromUtf8("Партия"));
    setSuffix("inner join rcp_nam on rcp_nam.id = dosage.id_rcp");
    for (int i=0; i<5; i++){
        setColumnFlags(i,Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    setColumnFlags(5,Qt::ItemIsEditable | Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    setColumnFlags(6,Qt::ItemIsEditable | Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
}

void ModelDoz::refresh(QDate begDate, QDate endDate, bool byNam)
{
    setFilter("dosage.dat between '"+begDate.toString("yyyy-MM-dd")+"' and '"+endDate.toString("yyyy-MM-dd")+"'");
    setSort(byNam ? QString("rcp_nam.nam, dosage.dat, dosage.tm") : QString("dosage.dat, dosage.tm"));
    select();
}

QVariant ModelDoz::data(const QModelIndex &index, int role) const
{
    if (index.column()==5){
        if (role==Qt::CheckStateRole){
            return DbTableModel::data(index,Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;
        } else if (role==Qt::DisplayRole){
            return DbTableModel::data(index,Qt::EditRole).toBool() ? QString::fromUtf8("Да") : QString::fromUtf8("Нет");
        } else if (role==Qt::TextAlignmentRole){
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    return DbTableModel::data(index,role);
}

void ModelDoz::confirmDoz(int row)
{
    if (setData(index(row,5),1,Qt::EditRole)){
        if (submit()){
            int id_doz=data(index(row,0),Qt::EditRole).toInt();
            QSqlQuery query;
            query.prepare("update dosage_spnd set kvo_fact = kvo_comp where id_dos = :id ");
            query.bindValue(":id",id_doz);
            if (!query.exec()){
                QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
            }
        }
    }
}

bool ModelDoz::deleteDb(int row)
{
    int id_doz=data(index(row,0),Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("delete from dosage_spnd where id_dos = :id ");
    query.bindValue(":id",id_doz);
    if (query.exec()){
        return DbTableModel::deleteDb(row);
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
        return false;
    }
}

ModelDozData::ModelDozData(QObject *parent): DbTableModel("dosage_spnd",parent)
{
    addColumn("id_dos",QString::fromUtf8("id_dos"));
    addColumn("id_comp",QString::fromUtf8("Компонент"),NULL,Rels::instance()->relComp);
    addColumn("id_bunk",QString::fromUtf8("Бункер"),NULL,Rels::instance()->relBunk);
    addColumn("parti",QString::fromUtf8("Партия"));
    addColumn("kvo_comp",QString::fromUtf8("Рецепт., кг"),new QDoubleValidator(0,1000000,2,this));
    addColumn("kvo_fact",QString::fromUtf8("Факт., кг"),new QDoubleValidator(0,1000000,2,this));
    addColumn("id_rcp",QString::fromUtf8("Рецепт. отходов"),NULL,Rels::instance()->relRcp);
    addColumn("parti_man",QString::fromUtf8("Руч. парт."));
    setSuffix("inner join matr on matr.id = dosage_spnd.id_comp");
    setSort("matr.nam");
}

QVariant ModelDozData::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundRole && index.column()==3 && data(this->index(index.row(),7),Qt::EditRole).toBool()){
        return QVariant(QColor(170,255,170));
    }
    return DbTableModel::data(index,role);
}

bool ModelDozData::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column()==3){
        setData(this->index(index.row(),7),true,Qt::EditRole);
    }
    return DbTableModel::setData(index,value,role);
}

void ModelDozData::refresh(int id_doz)
{
    setFilter("dosage_spnd.id_dos = "+QString::number(id_doz));
    setDefaultValue(0,id_doz);
    select();
}

ModelDozNew::ModelDozNew(QObject *parent) : QSqlQueryModel(parent)
{
    currentIdRcp=-1;
    currentDateTime=QDateTime::currentDateTime();
    currentMas=0.0;
    newIdDoz=1;
}

void ModelDozNew::refresh(double mas, QDateTime datetime, int id_rcp)
{
    currentIdRcp=id_rcp;
    currentDateTime=datetime;
    currentMas=mas;
    QSqlQuery query;
    query.prepare("select nam, kvo, nbunk, parti, id_matr, id_bunk from calc_doz(:mas, :dt, :rcp)");
    query.bindValue(":mas",mas);
    query.bindValue(":dt",datetime);
    query.bindValue(":rcp",id_rcp);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
        clear();
    } else {
        setQuery(query);
        setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Компонент"));
        setHeaderData(1,Qt::Horizontal,QString::fromUtf8("К-во, кг"));
        setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Бункер"));
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Партия"));
    }
}

QVariant ModelDozNew::data(const QModelIndex &item, int role) const
{
    if (role==Qt::DisplayRole){
        if (item.column()==1){
            return QLocale().toString(QSqlQueryModel::data(item,role).toDouble(),'f',2);
        } else {
            return QSqlQueryModel::data(item,role);
        }
    } else if (role==Qt::TextAlignmentRole){
        if (item.column()==1 || item.column()==2){
            return int(Qt::AlignRight | Qt::AlignVCenter);
        } else {
            return QSqlQueryModel::data(item,role);
        }
    }
    return QSqlQueryModel::data(item,role);
}

bool ModelDozNew::writeRcp()
{
    bool ok=false;
    QSqlQuery query;
    query.prepare("insert into dosage (id_rcp, dat, tm, kvo_tot) values (:id, :dat, :time, :kvo) returning id");
    query.bindValue(":id",currentIdRcp);
    query.bindValue(":dat",currentDateTime.date());
    query.bindValue(":time",currentDateTime.time());
    query.bindValue(":kvo",currentMas);
    if (query.exec()){
        while (query.next()){
            newIdDoz=query.value(0).toInt();
        }
        ok=writeDozData(newIdDoz);
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
    return ok;
}

int ModelDozNew::getNewId()
{
    return newIdDoz;
}

bool ModelDozNew::writeDozData(int id_doz)
{
    bool ok=true;
    for (int i=0; i<rowCount(); i++){
        QSqlQuery query;
        query.prepare("insert into dosage_spnd (id_dos, id_comp, kvo_comp, id_bunk, parti) values (:id_dos, :id_comp, :kvo, :id_bunk, :parti)");
        query.bindValue(":id_dos",id_doz);
        query.bindValue(":id_comp",data(index(i,4),Qt::EditRole).toInt());
        query.bindValue(":kvo",data(index(i,1),Qt::EditRole).toDouble());
        query.bindValue(":id_bunk",data(index(i,5),Qt::EditRole).toInt());
        query.bindValue(":parti",data(index(i,3),Qt::EditRole).toString());
        bool b=query.exec();
        ok=ok && b;
    }
    return ok;
}

ModelReport::ModelReport(QObject *parent) : QSqlQueryModel(parent)
{
    sumrcp=0.0;
    sumfact=0.0;
}

QVariant ModelReport::data(const QModelIndex &item, int role) const
{
    if (item.row()==QSqlQueryModel::rowCount()){
        if (role==Qt::EditRole || role==Qt::DisplayRole){
            if (item.column()==0){
                return QString::fromUtf8("Итого");
            } else if (item.column()==2 || item.column()==3){
                double sum = (item.column()==2) ? sumrcp : sumfact;
                if (role==Qt::EditRole){
                    return sum;
                } else if (role==Qt::DisplayRole){
                    return QLocale().toString(sum,'f',2);
                }
            }
        } else if (role==Qt::TextAlignmentRole){
            if (item.column()==2 || item.column()==3){
                return int(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                return int(Qt::AlignLeft | Qt::AlignVCenter);
            }
        }
        return QVariant();
    } else {
        if (role==Qt::DisplayRole){
            if (item.column()==2 || item.column()==3){
                return QLocale().toString(QSqlQueryModel::data(item,role).toDouble(),'f',2);
            } else {
                return QSqlQueryModel::data(item,role);
            }
        } else if (role==Qt::TextAlignmentRole){
            if (item.column()==2 || item.column()==3){
                return int(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                return QSqlQueryModel::data(item,role);
            }
        }
    }
    return QSqlQueryModel::data(item,role);
}

void ModelReport::refresh(QDate beg, QDate end, bool by_part)
{
    QSqlQuery query;
    QString q;
    q=by_part? QString("select m.nam, d.parti as part, sum(d.kvo_comp) as pln, sum(d.kvo_fact) as fct "
                       "from dosage_spnd d inner join matr m on m.id=d.id_comp "
                       "inner join dosage g on g.id = d.id_dos where g.dat between :d1 and :d2 "
                       "group by nam, part order by nam") :
               QString("select m.nam, NULL, sum(d.kvo_comp) as pln, sum(d.kvo_fact) as fct "
                       "from dosage_spnd d inner join matr m on m.id=d.id_comp "
                       "inner join dosage g on g.id = d.id_dos where g.dat between :d1 and :d2 "
                       "group by nam order by nam");
    query.prepare(q);
    query.bindValue(":d1",beg);
    query.bindValue(":d2",end);
    sumrcp=0.0;
    sumfact=0.0;
    if (query.exec()){
        setQuery(query);
        setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Компонент"));
        setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Партия"));
        setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Рецепт., кг"));
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Фактич., кг"));
        for (int i=0; i<QSqlQueryModel::rowCount(); i++){
            QModelIndex ircp=this->index(i,2);
            QModelIndex ifact=this->index(i,3);
            sumrcp+=(QSqlQueryModel::data(ircp).toDouble());
            sumfact+=(QSqlQueryModel::data(ifact).toDouble());
        }
    } else {
        clear();
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
}

int ModelReport::rowCount(const QModelIndex &parent) const
{
    return QSqlQueryModel::rowCount(parent)+1;
}

ModelCurrentBunk::ModelCurrentBunk(QObject *parent) : QSqlQueryModel(parent)
{

}

QVariant ModelCurrentBunk::data(const QModelIndex &item, int role) const
{
    if (item.column()==4 && role==Qt::DisplayRole){
        return QSqlQueryModel::data(item,role).toDateTime().toString("dd.MM.yy hh:mm");
    }
    return QSqlQueryModel::data(item,role);
}

void ModelCurrentBunk::refresh(QDateTime datetime)
{
    QSqlQuery query;
    query.prepare("select num, nam, parti, typ, dtm, is_tiny from calc_doz_stat(:dt) order by num, nam");
    query.bindValue(":dt",datetime);
    if (query.exec()){
        setQuery(query);
        setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Бункер"));
        setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Компонент"));
        setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Партия"));
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Группа"));
        setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Дата засыпки"));
    } else {
        clear();
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
}

ModelLoadBunk::ModelLoadBunk(QObject *parent) : DbTableModel("bunk_comp",parent)
{
    addColumn("id",QString::fromUtf8("id"));
    addColumn("dat",QString::fromUtf8("Дата"));
    addColumn("tm",QString::fromUtf8("Время"));
    addColumn("id_bunk",QString::fromUtf8("Бункер"),NULL,Rels::instance()->relBunk);
    addColumn("id_comp",QString::fromUtf8("Компонент"),NULL,Rels::instance()->relComp);
    addColumn("parti",QString::fromUtf8("Партия"));
    addColumn("id_grp",QString::fromUtf8("Группа"),NULL,Rels::instance()->relGrp);
    addColumn("id_op",QString::fromUtf8("Операция"),NULL,Rels::instance()->relOp);
    setSort("bunk_comp.dat, bunk_comp.tm");

    setDefaultValue(2,QTime::currentTime());
    setDefaultValue(7,1);
}

void ModelLoadBunk::refresh(QDate beg, QDate end)
{
    setFilter("bunk_comp.dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'");
    select();
}

bool ModelLoadBunk::updatePart(QDate beg, QDate end)
{
    QSqlQuery query;
    query.prepare("select * from calc_doz_parti_recalc(:d1, :d2)");
    query.bindValue(":d1",beg);
    query.bindValue(":d2",end);
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
    return ok;
}

ModelHist::ModelHist(QObject *parent) : QSqlQueryModel(parent)
{
}

QVariant ModelHist::data(const QModelIndex &item, int role) const
{
    if (item.column()==0 && role==Qt::DisplayRole){
        return QSqlQueryModel::data(item,role).toDateTime().toString("dd.MM.yy hh:mm");
    }
    return QSqlQueryModel::data(item,role);
}

void ModelHist::refresh(int id_matr, QDateTime tm)
{
    QSqlQuery query;
    query.prepare("select bc.dtm as dtm, b.numer, bc.parti, tp.nam as typ, bo.nam from bunk_comp as bc "
                  "inner join bunk as b on b.id=bc.id_bunk "
                  "inner join matr as m on m.id=bc.id_comp "
                  "inner join el_types as tp on tp.id=bc.id_grp "
                  "inner join bunk_op as bo on bo.id=bc.id_op "
                  "where bc.dtm>=:dt and bc.id_comp= :id_matr "
                  "order by dtm desc");
    query.bindValue(":dt",tm);
    query.bindValue(":id_matr",id_matr);
    if (query.exec()){
        setQuery(query);
        setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Дата засыпки"));
        setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Бункер"));
        setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Партия"));
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Группа"));
        setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Операция"));
        emit sigRefresh();
    } else {
        clear();
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
}

ModelGroupEl::ModelGroupEl(QObject *parent) : DbTableModel("elrtr",parent)
{
    addColumn("id",QString::fromUtf8("id"));
    addColumn("marka",QString::fromUtf8("Марка"));
    addColumn("id_vid_doz",QString::fromUtf8("Группа"),NULL,Rels::instance()->relGrp);
    setSort("elrtr.marka");
    setFilter("elrtr.id<>0");
    setColumnFlags(0,Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    setColumnFlags(1,Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    setColumnFlags(2,Qt::ItemIsEditable | Qt::ItemIsSelectable  | Qt::ItemIsEnabled);
    select();
}

bool ModelGroupEl::insertRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}

bool ModelGroupEl::removeRow(int /*row*/, const QModelIndex &/*parent*/)
{
    return false;
}
