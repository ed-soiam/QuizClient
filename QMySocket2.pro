#-------------------------------------------------
#
# Project created by QtCreator 2017-06-12T17:32:16
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++14
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#Имя проекта
TARGET = QuizClient
#Задает разновидность проекта: app - приложение, lib - библиотека, subdirs - поддиректория
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# Опции для компилятора
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    myclient.cpp \
    qmywidget.cpp \
    mytcpsocket.cpp \
    TransportParser/QJSONParser.cpp \
    TransportParser/QSimpleCommandParser.cpp \
    TransportParser/QTransportParser.cpp \
    TransportParser/QX25Parser.cpp \
    qtask/qjsontask.cpp \
    qtask/qtask.cpp \
    myThreadSocket/qshmanagerinfo.cpp \
    myThreadSocket/qthreadobject.cpp \
    myThreadSocket/QThreadSocket.cpp \
    qtwraptextpushbutton.cpp

HEADERS  += \
    myclient.h \
    qmywidget.h \
    mytcpsocket.h \
    TransportParser/QJSONParser.h \
    TransportParser/QSimpleCommandParser.h \
    TransportParser/QTransportParser.h \
    TransportParser/QX25Parser.h \
    qtask/qjsontask.h \
    qtask/qtask.h \
    myThreadSocket/qshmanagerinfo.h \
    myThreadSocket/qthreadobject.h \
    myThreadSocket/QThreadSocket.h \
    qtwraptextpushbutton.h

RESOURCES += \
    res.qrc

