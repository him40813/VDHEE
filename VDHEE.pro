#-------------------------------------------------
#
# Project created by QtCreator 2015-11-16T17:40:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VDHEE
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    videoprocess.cpp \
    bfm.cpp \
    tools.cpp \
    display.cpp \
    ffm.cpp \
    dialogrange.cpp \
    qlabelclick.cpp \
    cameracalibrator.cpp \
    settings.cpp \
    calibrater.cpp
INCLUDEPATH += C:/opencv/build2/install/include
LIBS += "C:/opencv/build2/bin/*.dll"

HEADERS  += mainwindow.h \
    bfm.h \
    videoProcess.h \
    tools.h \
    display.h \
    ffm.h \
    dialogrange.h \
    qlabelclick.h \
    settings.h \
    calibrater.h

FORMS    += mainwindow.ui \
    dialogrange.ui \
    calibratedialog.ui





