#-------------------------------------------------
#
# Project created by QtCreator 2021-03-19T04:36:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MQTT_Filter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    commonruledialog.cpp \
    helper.cpp

HEADERS  += mainwindow.h \
    commonruledialog.h \
    header.h \
    helper.h

FORMS    += mainwindow.ui \
    commonruledialog.ui
