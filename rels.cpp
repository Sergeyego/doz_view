#include "rels.h"

Rels* Rels::rels_instance=nullptr;

Rels *Rels::instance()
{
    if (rels_instance==nullptr)
        rels_instance = new Rels();
    return rels_instance;
}

Rels::Rels(QObject *parent) : QObject(parent)
{
    relRcp = new DbRelation(QString("select id, nam, lev from rcp_nam order by nam"),0,1,this);
    relComp = new DbRelation(QString("select id, nam from matr order by nam"),0,1,this);
    relBunk = new DbRelation(QString("select id, numer, is_tiny from bunk order by is_tiny, numer"),0,1,this);

    relRcp->proxyModel()->setFilterKeyColumn(2);
    relRcp->proxyModel()->setFilterFixedString("1");
}

void Rels::refresh()
{
    relRcp->model()->refresh();
    relComp->model()->refresh();
    relBunk->model()->refresh();
    emit sigRefresh();
}
