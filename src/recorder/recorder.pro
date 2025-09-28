# https://itnotesblog.ru/note/struktura-qt-proekta-na-cpp

include( ../../common.pri )
include( ../../app.pri )

QT = core multimedia network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../include/linkerudp.cpp \
        ../include/savewav.cpp \
        a_buffer.cpp \
        a_dispatcher.cpp \
        a_init.cpp \
        a_recorder.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    areader.ini 

HEADERS += \
    ../include/a_codelist.h \
    ../include/bm_codelist.h \
    ../include/data_structures.h \
    ../include/linkerudp.h \
    ../include/savewav.h \	
    ../../import/ini.h \
    a_buffer.h \
    a_dispatcher.h \
    a_init.h \
    a_recorder.h 
	
	
#LIBS += -lMyLib$${LIB_SUFFIX}	
	
win64 {
    QMAKE_TARGET_PRODUCT = AP_Recorder
    QMAKE_TARGET_DESCRIPTION = Audio_Channel_Data_Registrator
}

QMAKE_TARGET_PRODUCT = AP_Recorder
QMAKE_TARGET_DESCRIPTION = Audio_Channel_Data_Registrator
