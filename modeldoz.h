#ifndef MODELDOZ_H
#define MODELDOZ_H

#include <QObject>
#include "db/dbtablemodel.h"
#include "rels.h"

class ModelDoz : public DbTableModel
{
public:
    ModelDoz(QObject *parent=0);
    void refresh(QDate begDate, QDate endDate, bool byNam);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void confirmDoz(int row);

protected:
    bool deleteDb(int row);
};

class ModelDozData : public DbTableModel
{
public:
    ModelDozData(QObject *parent=0);
    void refresh(int id_doz);
};

class ModelDozNew : public QSqlQueryModel
{
public:
    ModelDozNew(QObject *parent=0);
    void refresh(double mas, QDateTime datetime, int id_rcp);
    QVariant data(const QModelIndex &item, int role) const;
    bool writeRcp();
    int getNewId();

private:
    int currentIdRcp;
    double currentMas;
    QDateTime currentDateTime;
    int newIdDoz;
    bool writeDozData(int id_doz);
};

class ModelReport: public QSqlQueryModel
{
public:
    ModelReport(QObject *parent=0);
    QVariant data(const QModelIndex &item, int role) const;
    void refresh(QDate beg, QDate end, bool by_part);
    int rowCount(const QModelIndex &parent) const;
private:
    double sumrcp;
    double sumfact;
};

class ModelCurrentBunk: public QSqlQueryModel
{
public:
    ModelCurrentBunk(QObject *parent=0);
    QVariant data(const QModelIndex &item, int role) const;
    void refresh(QDateTime datetime);
};

class ModelCurrentPart: public QSqlQueryModel
{
public:
    ModelCurrentPart(QObject *parent=0);
    QVariant data(const QModelIndex &item, int role) const;
    void refresh(QDateTime datetime);
};

class ModelLoadBunk : public DbTableModel
{
public:
    ModelLoadBunk(QObject *parent=0);
    void refresh(QDate beg, QDate end);
    bool updatePart(QDate beg, QDate end);
};
#endif // MODELDOZ_H
