#-------------------------------------------------
#
# Project created by QtCreator 2019-11-21T09:17:09
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = doz_view
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    modeldoz.cpp \
    rels.cpp \
    dialognew.cpp \
    olap/axiswidget.cpp \
    olap/cubewidget.cpp \
    olap/cubic.cpp \
    olap/olapmodel.cpp \
    olap/tableview.cpp \
    dialogreport.cpp \
    dialogbunk.cpp \
    dialoghist.cpp \
    dialoggroup.cpp \
    db/dbsave.cpp

HEADERS += \
        mainwindow.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    modeldoz.h \
    rels.h \
    dialognew.h \
    olap/axiswidget.h \
    olap/cubewidget.h \
    olap/cubic.h \
    olap/olapmodel.h \
    olap/tableview.h \
    dialogreport.h \
    dialogbunk.h \
    dialoghist.h \
    dialoggroup.h \
    db/dbsave.h

FORMS += \
        mainwindow.ui \
    db/dblogin.ui \
    dialognew.ui \
    olap/axiswidget.ui \
    olap/cubewidget.ui \
    dialogreport.ui \
    dialogbunk.ui \
    dialoghist.ui \
    dialoggroup.ui

DISTFILES +=

INCLUDEPATH+=/usr/include/xlslib
LIBS += -lxls

RESOURCES += \
    res.qrc
