# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++20

TEMPLATE = app

SOURCES +=  tst_tdb_test1.cpp
