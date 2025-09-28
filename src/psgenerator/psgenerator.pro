# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT = core network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../include/linkerudp.cpp \
        ../include/savewav.cpp \
        ../include/initunit.cpp \
        main.cpp \
        psg_dispatcher.cpp \
        psg_generator.cpp \
        psg_init.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../include/bm_codelist.h \
    ../include/data_structures.h \
    ../include/linkerudp.h \
    ../include/savewav.h \
    ../include/initunit.h \
    ../../import/ini.h \
    psg_dispatcher.h \
    psg_generator.h \
    psg_init.h
