
include( ../../common.pri )
include( ../../app.pri )

QT -= gui
QT += testlib
#QT += core

TARGET = tst_rbuffer
#CONFIG += qt warn_on depend_includepath testcase
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#INCLUDEPATH += $${SOURCE_PATH}/

HEADERS = ../../src/include/rbuffer.h \

SOURCES =  tst_rbuffer.cpp\

