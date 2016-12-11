#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T12:13:01
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport
QT  += serialport
QT += widgets
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AFRmeter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui
