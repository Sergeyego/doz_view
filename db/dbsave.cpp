#include "dbsave.h"

DbSave::DbSave(QTableView *v, QObject *parent) : QObject(parent)
{
    viewer=v;
}

void DbSave::save(QString fnam)
{
    if (!viewer->model()) return;
    int rows,cols;
    int i,j;
    rows=viewer->model()->rowCount();
    cols=viewer->model()->columnCount();

    if (rows*cols>1){
        workbook wb;
        worksheet *sh = wb.sheet("sheet");
        cell_t *cref;

        xf_t * fmt = wb.xformat();
        fmt->SetBorderStyle(BORDER_LEFT, BORDER_THIN);
        fmt->SetBorderStyle(BORDER_RIGHT, BORDER_THIN);
        fmt->SetBorderStyle(BORDER_TOP, BORDER_THIN);
        fmt->SetBorderStyle(BORDER_BOTTOM, BORDER_THIN);

        QString hCubeell;
        sh->label(0,0,fnam.toStdWString());
        cref=sh->FindCellOrMakeBlank(0,0);
        cref->fontbold(BOLDNESS_DOUBLE);
        sh->rowheight(0,300);

        sh->rowheight(1,(viewer->horizontalHeader()->sizeHint().height())*30);

        sh->colwidth(0,(viewer->verticalHeader()->sizeHint().width())*38);

        sh->blank(1,0,fmt);

        int v=viewer->verticalHeader()->isVisible() ? 1 : 0;

        int m=0;
        for(i=0;i<cols;i++){
            if (!viewer->isColumnHidden(i)) {
                hCubeell=viewer->model()->headerData(i,Qt::Horizontal).toString();
                hCubeell.replace(QChar('\n'),QChar('\n'));
                sh->label(1,m+v,hCubeell.toStdWString(),fmt);
                cref=sh->FindCellOrMakeBlank(1,m+v);
                cref->fontbold(BOLDNESS_DOUBLE);
                cref->halign(HALIGN_JUSTIFY);
                sh->colwidth(m+v,(viewer->columnWidth(i)*38));
                m++;
            }
        }

        if (viewer->verticalHeader()->isVisible()){
            for(j=0;j<rows;j++){
                sh->label(j+2,0,viewer->model()->headerData(j,Qt::Vertical).toString().toStdWString(),fmt);
                cref=sh->FindCellOrMakeBlank(j+2,0);
                cref->halign(HALIGN_LEFT);
            }
        }

        DbTableModel *sqlModel = qobject_cast<DbTableModel *>(viewer->model());

        for (i=0;i<rows;i++){
            m=0;
            for(j=0;j<cols;j++){
                if (!viewer->isColumnHidden(j)) {
                    xf_t * pxf = wb.xformat();
                    pxf->SetBorderStyle(BORDER_LEFT, BORDER_THIN);
                    pxf->SetBorderStyle(BORDER_RIGHT, BORDER_THIN);
                    pxf->SetBorderStyle(BORDER_TOP, BORDER_THIN);
                    pxf->SetBorderStyle(BORDER_BOTTOM, BORDER_THIN);
                    QVariant vdisp=viewer->model()->data(viewer->model()->index(i,j),Qt::DisplayRole);
                    QVariant vedt=viewer->model()->data(viewer->model()->index(i,j),Qt::EditRole);

                    QString tname=vedt.typeName();

                    if (sqlModel){
                        if (sqlModel->relation(j) || sqlModel->columnType(j)==QMetaType::Bool || sqlModel->columnType(j)==QMetaType::QDate || sqlModel->columnType(j)==QMetaType::QDateTime){
                            tname=vdisp.typeName();
                        }
                    }

                    if (tname==QString("int")){
                        pxf->SetFormat(FMT_NUMBER3);
                    } else if (tname==QString("double")){
                        pxf->SetFormat(FMT_NUMBER4);
                    } else {
                        pxf->SetFormat(FMT_GENERAL);
                    }
                    if (!vdisp.toString().isEmpty()){
                        if (tname==QString("double") || tname==QString("int")){
                            sh->number(i+2,m+v,vedt.toDouble(),pxf);
                        } else {
                            sh->label(i+2,m+v,vdisp.toString().toStdWString(),pxf);
                        }
                    } else {
                        sh->blank(i+2,m+v,pxf);
                    }
                    m++;
                }
            }
        }

        QSettings settings("szsm", QApplication::applicationName());
        QDir dir(settings.value("savePath",QDir::homePath()).toString());
        QString filename = QFileDialog::getSaveFileName(NULL,QString::fromUtf8("Сохранить документ"),
                                                        dir.path()+"/"+fnam+".xls",
                                                        QString::fromUtf8("Documents (*.xls)") );
        if (!filename.isEmpty()){
            QFile file(filename);
            if (file.exists()) file.remove();
            std::string fil(filename.toLocal8Bit());
            wb.Dump(fil);
            QFileInfo info(file);
            settings.setValue("savePath",info.path());
        }
    }
}
