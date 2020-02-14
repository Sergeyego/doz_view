#ifndef DBSAVE_H
#define DBSAVE_H

#include <QObject>
#include <QTableView>
#include <QHeaderView>
#include <xlslib.h>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include "dbtablemodel.h"

using namespace xlslib_core;

class DbSave : public QObject
{
    Q_OBJECT
public:
    explicit DbSave(QTableView *v, QObject *parent = 0);
private:
    QTableView *viewer;

signals:

public slots:
    void save(QString fnam);
};

#endif // DBSAVE_H
