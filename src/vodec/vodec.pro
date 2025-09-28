# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT = core network

CONFIG += c++20 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../../../libs/eigen-3.4.0/

SOURCES += \
        ../include/coreunit.cpp \
        ../include/dispunit.cpp \
        ../include/initunit.cpp \
        ../include/linkerudp.cpp \
        ../include/sp/tdbuffer.cpp \
        main.cpp \
        vd_core.cpp \
        vd_init.cpp \
        vd_unit.cpp \
        vd_voicedetector.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../import/AudioFile.h \
    ../include/bm_codelist.h \
    ../include/coreunit.h \
    ../include/data_structures.h \
    ../include/dispunit.h \
    ../include/initunit.h \
    ../include/linkerudp.h \
    ../include/protounit.h \
    ../include/sp/fdbuffer.h \
    ../include/sp/n_model.h \
    ../include/sp/spm.h \
    ../include/sp/tdbuffer.h \
    ../include/sp/vocaldetector.h \
    tdbuffer.h \
    vd_artifanalizer.h \
    vd_coef.h \
    vd_core.h \
    vd_init.h \
    vd_unit.h \
    vd_voicedetector.h
