#-------------------------------------------------
#
# Project created by QtCreator 2016-04-14T13:12:31
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myServer
TEMPLATE = app


SOURCES += main.cpp\
        myserver.cpp

HEADERS  += myserver.h

FORMS    += myserver.ui
