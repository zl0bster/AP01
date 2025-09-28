# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT       += core gui widgets network multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../include/linkerudp.cpp \
    bandmaster.cpp \
    bm_dispatcher.cpp \
    bm_handlergeneral.cpp \
    bm_handlerpsgenerator.cpp \
    bm_handlerrecorder.cpp \
    bm_handlersplayer.cpp \
    bm_hfactory.cpp \
    bm_hstatemachine.cpp \
    bm_mastergeneral.cpp \
    bm_masterpsgen.cpp \
    bm_masterrecord.cpp \
    bm_mastersplayer.cpp \
    bm_watchmangeneral.cpp \
    main.cpp \
    bandmastergui.cpp

HEADERS += \
    ../include/a_codelist.h \
    ../include/bm_codelist.h \
    ../include/data_structures.h \
    ../include/linkerudp.h \
    ../include/bm_guihandles.h \
    ../../import/ini.h \
    ../include/rbuffer.h \
    bandmaster.h \
    bandmastergui.h \
    bm_dispatcher.h \
    bm_handlergeneral.h \
    bm_handlerpsgenerator.h \
    bm_handlerrecorder.h \
    bm_handlersplayer.h \
    bm_hfactory.h \
    bm_hstatemachine.h \
    bm_mastergeneral.h \
    bm_masterpsgen.h \
    bm_masterrecord.h \
    bm_mastersplayer.h \
    bm_statemap.h \
    bm_watchmangeneral.h

FORMS += \
    bandmastergui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    bmhfactory.ini
	
#LIBS += -lMyLib$${LIB_SUFFIX}	
	
win64 {
    QMAKE_TARGET_PRODUCT = AP_BandMaster
    QMAKE_TARGET_DESCRIPTION = HMI_to_AP
}

QMAKE_TARGET_PRODUCT = AP_BandMaster
QMAKE_TARGET_DESCRIPTION = HMI_to_AP
