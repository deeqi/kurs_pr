#-------------------------------------------------
#
# Project created by QtCreator 2016-04-23T16:11:59
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myClient
TEMPLATE = app


SOURCES += main.cpp\
        myclient.cpp \
    result.cpp \
    basket.cpp

HEADERS  += myclient.h \
    result.h \
    basket.h

FORMS    += myclient.ui \
    result.ui \
    basket.ui
