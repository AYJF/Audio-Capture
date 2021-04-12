#-------------------------------------------------
#
# Project created by QtCreator 2011-09-22T14:10:36
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = AudioCapture
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

INCLUDEPATH += ./fft
LIBS += -L./fft -lfftw3-3
