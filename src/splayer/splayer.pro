# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT = core multimedia network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../include/coreunit.cpp \
        ../include/dispunit.cpp \
        ../include/initunit.cpp \
        ../include/linkerudp.cpp \
        main.cpp \
        sp_core.cpp \
        sp_player.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../include/bm_codelist.h \
    ../include/coreunit.h \
    ../include/data_structures.h \
    ../include/dispunit.h \
    ../include/initunit.h \
    ../include/linkerudp.h \
    ../include/protounit.h \
    sp_core.h \
    sp_player.h

#LIBS += -lMyLib$${LIB_SUFFIX}

win64 {
    QMAKE_TARGET_PRODUCT = AP_Recorder
    QMAKE_TARGET_DESCRIPTION = Audio_Channel_Data_Registrator
}

QMAKE_TARGET_PRODUCT = AP_Recorder
QMAKE_TARGET_DESCRIPTION = Audio_Channel_Data_Registrator
