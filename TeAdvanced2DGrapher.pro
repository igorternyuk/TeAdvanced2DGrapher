#-------------------------------------------------
#
# Project created by QtCreator 2017-07-26T15:39:12
#
#-------------------------------------------------

QT += core gui charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TeAdvanced2DGrapher
TEMPLATE = app

DEFINES += DEBUG

SOURCES += \
    chartviewwithzoom.cpp \
    customchart.cpp \
    dialogdiagramsettings.cpp \
    doublespinboxdelegate.cpp \
    main.cpp \
    matematica.cpp \
    matrix.cpp \
    parser.cpp \
    qcustomplot.cpp \
    widget.cpp

HEADERS  += \
    chartviewwithzoom.h \
    customchart.h \
    dialogdiagramsettings.h \
    doublespinboxdelegate.h \
    matematica.h \
    matrix.h \
    parser.h \
    polynom.h \
    qcustomplot.h \
    widget.h

FORMS    += \
    dialogdiagramsettings.ui \
    widget.ui

QMAKE_CXXFLAGS += -std=c++14
