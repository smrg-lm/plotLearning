#-------------------------------------------------
#
# Project created by QtCreator 2018-01-09T23:35:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plotLearning
TEMPLATE = app

CONFIG += c++11 strict_c++

SOURCES += main.cpp\
    workarea.cpp \
    plotwidget.cpp \
    plotview.cpp \
    scrollview.cpp \
    abstractview.cpp \
    rulearea.cpp \
    visualgroup.cpp \
    visualelement.cpp \
    visualwave.cpp \
    controlpoint.cpp

HEADERS  += \
    workarea.h \
    plotwidget.h \
    plotview.h \
    scrollview.h \
    abstractview.h \
    rulearea.h \
    visualgroup.h \
    visualelement.h \
    visualwave.h \
    controlpoint.h
