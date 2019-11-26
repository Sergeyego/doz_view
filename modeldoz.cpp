#include "modeldoz.h"

ModelDoz::ModelDoz(QObject *parent) : DbTableModel("dosage",parent)
{
    addColumn("id",QString::fromUtf8("id"),true,TYPE_INT);
    addColumn("dat",QString::fromUtf8("Дата"),false,TYPE_DATE);
    addColumn("tm",QString::fromUtf8("Время"),false,TYPE_VARIANT);
    addColumn("id_rcp",QString::fromUtf8("Рецептура"),false,TYPE_STRING,NULL,Rels::instance()->relRcp);
    addColumn("kvo_tot",QString::fromUtf8("К-во, кг"),false,TYPE_DOUBLE,new QDoubleValidator(0,1000000,1,this));
    addColumn("result",QString::fromUtf8("OK"),false,TYPE_INTBOOL);
    addColumn("parti",QString::fromUtf8("Партия"),false,TYPE_STRING);
    setSuffix("inner join rcp_nam on rcp_nam.id = dosage.id_rcp");
}

void ModelDoz::refresh(QDate begDate, QDate endDate, bool byNam)
{
    setFilter("dosage.dat between '"+begDate.toString("yyyy-MM-dd")+"' and '"+endDate.toString("yyyy-MM-dd")+"'");
    setSort(byNam ? QString("rcp_nam.nam, dosage.dat, dosage.tm") : QString("dosage.dat, dosage.tm"));
    select();
}

Qt::ItemFlags ModelDoz::flags(const QModelIndex &index) const
{
    if (index.column()==5){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable  | Qt::ItemIsEnabled;
    } else if (index.column()==6){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    } else {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

void ModelDoz::confirmDoz(int row)
{
    if (setData(index(row,5),1,Qt::EditRole)){
        if (submitRow()){
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
    addColumn("id_dos",QString::fromUtf8("id_dos"),true,TYPE_INT);
    addColumn("id_comp",QString::fromUtf8("Компонент"),true,TYPE_STRING,NULL,Rels::instance()->relComp);
    addColumn("id_bunk",QString::fromUtf8("Бункер"),true,TYPE_STRING,NULL,Rels::instance()->relBunk);
    addColumn("parti",QString::fromUtf8("Партия"),false,TYPE_STRING);
    addColumn("kvo_comp",QString::fromUtf8("Рецепт., кг"),false,TYPE_DOUBLE,new QDoubleValidator(0,1000000,2,this));
    addColumn("kvo_fact",QString::fromUtf8("Факт., кг"),false,TYPE_DOUBLE,new QDoubleValidator(0,1000000,2,this));
    addColumn("id_rcp",QString::fromUtf8("Рецепт. отходов"),false,TYPE_STRING,NULL,Rels::instance()->relRcp);
    setSuffix("inner join matr on matr.id = dosage_spnd.id_comp");
    setSort("matr.nam");
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
        setHeaderData(0,Qt::Horizontal,QString("Компонент"));
        setHeaderData(1,Qt::Horizontal,QString("К-во, кг"));
        setHeaderData(2,Qt::Horizontal,QString("Бункер"));
        setHeaderData(3,Qt::Horizontal,QString("Партия"));
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

}

QVariant ModelReport::data(const QModelIndex &item, int role) const
{
    if (item.row()==QSqlQueryModel::rowCount()){
        if (role==Qt::EditRole || role==Qt::DisplayRole){
            if (item.column()==0){
                return QString("Итого");
            } else if (item.column()==2 || item.column()==3){
                double sum=0.0;
                for (int i=0; i<QSqlQueryModel::rowCount(); i++){
                    QModelIndex cs=this->index(i,item.column());
                    sum+=(QSqlQueryModel::data(cs).toDouble());
                }
                if (role==Qt::EditRole){
                    return sum;
                } else if (role==Qt::DisplayRole){
                    return QLocale().toString(sum,'f',2);
                } else if (role==Qt::TextAlignmentRole){
                    return int(Qt::AlignRight | Qt::AlignVCenter);
                }
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
    if (query.exec()){
        setQuery(query);
        setHeaderData(0,Qt::Horizontal,QString("Компонент"));
        setHeaderData(1,Qt::Horizontal,QString("Партия"));
        setHeaderData(2,Qt::Horizontal,QString("Рецепт., кг"));
        setHeaderData(3,Qt::Horizontal,QString("Фактич., кг"));
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

void ModelCurrentBunk::refresh(QDateTime datetime)
{
    QSqlQuery query;
    query.prepare("select bk.numer, t.nam, t.parti from bunk as bk "
                  "left join "
                  "(select bc.id_bunk, m.nam, bc.parti from bunk_comp as bc "
                  "inner join bunk as b on b.id=bc.id_bunk "
                  "inner join matr as m on m.id=bc.id_comp "
                  "where bc.dtm=(select max(dtm) from bunk_comp as bcc where bcc.dtm <= :dt and bcc.id_bunk=bc.id_bunk) "
                  "group by bc.id_bunk, m.nam, bc.parti) as t on t.id_bunk=bk.id "
                  "where bk.is_tiny=0 "
                  "order by bk.nomer");
    query.bindValue(":dt",datetime);
    if (query.exec()){
        setQuery(query);
        setHeaderData(0,Qt::Horizontal,QString("Бункер"));
        setHeaderData(1,Qt::Horizontal,QString("Компонент"));
        setHeaderData(2,Qt::Horizontal,QString("Партия"));
    } else {
        clear();
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
}

ModelLoadBunk::ModelLoadBunk(QObject *parent) : DbTableModel("bunk_comp",parent)
{
    addColumn("id",QString::fromUtf8("id"),true,TYPE_INT);
    addColumn("dat",QString::fromUtf8("Дата"),false,TYPE_DATE);
    addColumn("tm",QString::fromUtf8("Время"),false,TYPE_VARIANT);
    addColumn("id_bunk",QString::fromUtf8("Бункер"),true,TYPE_STRING,NULL,Rels::instance()->relBunk);
    addColumn("id_comp",QString::fromUtf8("Компонент"),true,TYPE_STRING,NULL,Rels::instance()->relComp);
    addColumn("parti",QString::fromUtf8("Партия"),false,TYPE_STRING);
    setSort("bunk_comp.dat, bunk_comp.tm");

    setDefaultValue(2,QTime::currentTime());
}

void ModelLoadBunk::refresh(QDate beg, QDate end)
{
    setFilter("bunk_comp.dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'");
    select();
}
