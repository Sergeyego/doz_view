#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include "modeldoz.h"
#include "rels.h"
#include "dialognew.h"
#include "dialogreport.h"
#include "dialogbunk.h"
#include "olap/cubewidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    QButtonGroup *groupSort;
    QButtonGroup *groupReport;
    ModelDoz *modelDoz;
    ModelDozData *modelDozData;
    CubeWidget *cubeDoz;
    DialogReport *dialogReport;
    DialogBunk *dialogBunk;

private slots:
    void updDoz();
    void updDozData(QModelIndex index);
    void toEndDoz();
    void delDoz();
    void confirmDoz();
    void newDoz();
    void reportDoz();
};

#endif // MAINWINDOW_H
