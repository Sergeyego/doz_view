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

protected:
    explicit Rels(QObject *parent = nullptr);

private:
    static Rels *rels_instance;

signals:
    void sigRefresh();

public slots:
    void refresh();

};

#endif // RELS_H
