#ifndef RELS_H
#define RELS_H

#include <QObject>
#include "db/dbtablemodel.h"

class Rels : public QObject
{
    Q_OBJECT
public:
    static Rels *instance();
    DbRelation *relRcp;
    DbRelation *relComp;
    DbRelation *relBunk;
    DbRelation *relGrp;
    DbRelation *relOp;

protected:
    explicit Rels(QObject *parent = 0);

private:
    static Rels *rels_instance;

signals:
    void sigRefresh();

public slots:
    void refresh();

};

#endif // RELS_H
