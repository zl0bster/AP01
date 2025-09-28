# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT = core #network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../lib_win64/dspl.c \
#        ../include/coreunit.cpp \
#        ../include/dispunit.cpp \
#        ../include/initunit.cpp \
#        ../include/linkerudp.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
 #   ../../import/AudioFile.h \
 #   ../../import/ini.h \
    ../../lib_win64/dspl.h \
 #   ../include/bm_codelist.h \
 #   ../include/coreunit.h \
 #   ../include/data_structures.h \
 #   ../include/dispunit.h \
 #   ../include/initunit.h \
 #   ../include/linkerudp.h \
 \ #   ../include/protounit.h
    ../include/sp/tdbuffer.h

LIBS += libdspl

DISTFILES += \
    ../../lib_win64/libdspl.dll \
#    ../../lib_win64/libdspl.bin

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../release/ -ldspl
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug/ -ldspl

