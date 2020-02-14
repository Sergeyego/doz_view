#include "dbviewer.h"

DbViewer::DbViewer(QWidget *parent) :
    QTableView(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    menuEnabled=true;
    verticalHeader()->setDefaultSectionSize(verticalHeader()->fontMetrics().height()*1.5);
    verticalHeader()->setFixedWidth(verticalHeader()->fontMetrics().height()*1.2);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#endif

    updAct = new QAction(QString::fromUtf8("Обновить"),this);
    removeAct = new QAction(QString::fromUtf8("Удалить"),this);
    this->setAutoScroll(true);
    this->setItemDelegate(new DbDelegate(this));
    writeOk=true;
    connect(updAct,SIGNAL(triggered()),this,SLOT(upd()));
    connect(removeAct,SIGNAL(triggered()),this,SLOT(remove()));
}

void DbViewer::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    disconnect(selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this->model(), SLOT(submit()));
    connect(this->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(submit(QModelIndex,QModelIndex)));
}

void DbViewer::setColumnsWidth(QVector<int> width)
{
    for (int i=0; i<width.size(); i++){
        setColumnWidth(i,width[i]);
    }
}

void DbViewer::keyPressEvent(QKeyEvent *e)
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(this->model());
    if (sqlModel && this->editTriggers()!=QAbstractItemView::NoEditTriggers){
        int c=sqlModel->rowCount();
        int row=currentIndex().row();

        switch (e->key()){
            case Qt::Key_Delete:
            {
                if (e->modifiers()==Qt::ControlModifier) remove();
                break;
            }
            case Qt::Key_Escape:
            {
                sqlModel->revert();
                break;
            }
            case Qt::Key_Down:
            {
                if ((row==c-1) || !c){
                    if (sqlModel->isEdt()){
                        sqlModel->submit();
                    }
                    sqlModel->insertRow(sqlModel->rowCount());
                }
                QTableView::keyPressEvent(e);
                break;
            }
            case Qt::Key_Tab:
            {
                int j=sqlModel->columnCount()-1;
                bool hidden=isColumnHidden(j);
                while (hidden && j>0){
                    j--;
                    hidden=this->isColumnHidden(j);
                }

                int i=0;
                hidden=isColumnHidden(i);
                while (hidden && i<sqlModel->columnCount()){
                    i++;
                    hidden=this->isColumnHidden(i);
                }

                if ((currentIndex().column()==j) && (row==sqlModel->rowCount()-1)) {
                    if (sqlModel->isEdt()){
                        sqlModel->submit();
                    }
                    sqlModel->insertRow(sqlModel->rowCount());
                    QTableView::keyPressEvent(e);
                } else {
                    QTableView::keyPressEvent(e);
                }
                break;
            }
            default:
            {
                QTableView::keyPressEvent(e);
                break;
            }
        }

    } else {
        QTableView::keyPressEvent(e);
    }
}


void DbViewer::upd()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(this->model());
    if (sqlModel) {
        sqlModel->select();
    }
}

void DbViewer::remove()
{
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(this->model());
    QModelIndex ind=this->currentIndex();
    if (sqlModel && sqlModel->rowCount() && sqlModel->removeRow(ind.row())){
        if (ind.row()>0){
            setCurrentIndex(model()->index(ind.row()-1,ind.column()));
        } else if (sqlModel->rowCount()){
            setCurrentIndex(model()->index(ind.row(),ind.column()));
        }
    }
}

void DbViewer::submit(QModelIndex /*ind*/, QModelIndex oldInd)
{
    if (this->editTriggers()==QAbstractItemView::NoEditTriggers) return;
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(this->model());
    if (sqlModel){
        if (!writeOk){
            writeOk=true;
            return;
        }
        if (sqlModel->isEdt() || (sqlModel->isAdd() && oldInd.row()==sqlModel->currentEdtRow())){
            writeOk=sqlModel->submit();
        }
    }
}

void DbViewer::focusOutEvent(QFocusEvent *event)
{
    if (this->editTriggers()!=QAbstractItemView::NoEditTriggers && event->reason()==Qt::MouseFocusReason){
        DbTableModel *sqlModel = qobject_cast<DbTableModel *>(this->model());
        if (sqlModel && sqlModel->isAdd() && !sqlModel->isEdt()){
            sqlModel->revert();
        }
    }
    return QTableView::focusOutEvent(event);
}

void DbViewer::save(QString title)
{
    int rows,cols;
    int i,j;
    rows=this->model()->rowCount();
    cols=this->model()->columnCount();

    if (rows*cols>1){
        workbook wb;
        worksheet *sh = wb.sheet("sheet");
        cell_t *cref;

        QString hCubeell;

        //sh->merge(0,0,0,cols);
        sh->label(0,0,title.toStdWString());
        cref=sh->FindCellOrMakeBlank(0,0);
        cref->fontbold(BOLDNESS_DOUBLE);
        //cref->halign(HALIGN_CENTER);
        sh->rowheight(0,300);

        sh->rowheight(1,(this->horizontalHeader()->sizeHint().height())*30);

        sh->colwidth(0,(this->verticalHeader()->sizeHint().width())*38);

        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_LEFT, BORDER_HAIR);
        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_RIGHT, BORDER_HAIR);
        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_TOP, BORDER_HAIR);
        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_BOTTOM, BORDER_HAIR);

        for(i=0;i<cols;i++)
        {
            hCubeell=this->model()->headerData(i,Qt::Horizontal).toString();
            hCubeell.replace(QChar('\n'),QChar('\n'));
            sh->label(1,i+1,hCubeell.toStdWString());
            cref=sh->FindCellOrMakeBlank(1,i+1);

            cref->borderstyle(BORDER_LEFT, BORDER_HAIR);
            cref->borderstyle(BORDER_RIGHT, BORDER_HAIR);
            cref->borderstyle(BORDER_TOP, BORDER_HAIR);
            cref->borderstyle(BORDER_BOTTOM, BORDER_HAIR);

            cref->fontbold(BOLDNESS_DOUBLE);
            cref->halign(HALIGN_JUSTIFY);
            sh->colwidth(i+1,(this->columnWidth(i)*38));
        }

        for(j=0;j<rows;j++)
        {
            sh->label(j+2,0,this->model()->headerData(j,Qt::Vertical).toString().toStdWString());
            cref=sh->FindCellOrMakeBlank(j+2,0);
            cref->borderstyle(BORDER_LEFT, BORDER_HAIR);
            cref->borderstyle(BORDER_RIGHT, BORDER_HAIR);
            cref->borderstyle(BORDER_TOP, BORDER_HAIR);
            cref->borderstyle(BORDER_BOTTOM, BORDER_HAIR);
            cref->halign(HALIGN_LEFT);
        }

        DbTableModel *sqlModel = qobject_cast<DbTableModel *>(this->model());

        for (i=0;i<rows;i++)
            for(j=0;j<cols;j++)
            {
                int role=Qt::EditRole;
                if (sqlModel){
                    if (sqlModel->relation(j) || sqlModel->columnType(j)==QMetaType::Bool || sqlModel->columnType(j)==QMetaType::QDate || sqlModel->columnType(j)==QMetaType::QDateTime) {
                        role=Qt::DisplayRole;
                    }
                }
                QVariant value=this->model()->data(this->model()->index(i,j),role);
                if ((value.typeName()==QString("double"))||value.typeName()==QString("int")){
                    /*xf_t* f = wb.xformat();
                            f->SetFormat(FMT_NUMBER2);*/
                    sh->number(i+2,j+1,value.toDouble());
                } else {
                    value=this->model()->data(this->model()->index(i,j),Qt::DisplayRole).toString();
                    sh->label(i+2,j+1,value.toString().toStdWString());
                }
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_LEFT, BORDER_HAIR);
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_RIGHT, BORDER_HAIR);
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_TOP, BORDER_HAIR);
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_BOTTOM, BORDER_HAIR);
            }

        QDir dir(QDir::homePath()+"/provreport");
        if (!dir.exists()) dir.mkdir(dir.path());
        QString filename = QFileDialog::getSaveFileName(this,QString::fromUtf8("Сохранить документ"),
                                                        dir.path()+"/"+title+".xls",
                                                        QString::fromUtf8("Documents (*.xls)") );
        if (!filename.isEmpty()){
            QFile file(filename);
            if (file.exists()) file.remove();
            std::string fil(filename.toLocal8Bit());
            wb.Dump(fil);
        }
    }
}

void DbViewer::setMenuEnabled(bool value)
{
    menuEnabled=value;
}

void DbViewer::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    if (menuEnabled){
        menu.addAction(updAct);
        menu.addSeparator();
        if (this->selectionModel()){
            if (this->indexAt(event->pos()).isValid()){
                menu.addAction(removeAct);
                menu.addSeparator();
            }
        }
    }
    menu.exec(event->globalPos());
}

DateEdit::DateEdit(QWidget *parent): QDateEdit(parent)
{
    this->setCalendarPopup(true);
    QCalendarWidget * pCW = new QCalendarWidget(this);
    pCW->setFirstDayOfWeek( Qt::Monday );
    this->setCalendarWidget( pCW );
    this->setDisplayFormat("dd.MM.yy");
}
