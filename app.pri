DESTDIR = $${BIN_PATH}/
linux-g++: QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../../lib.$${OS_SUFFIX}/

QMAKE_TARGET_COPYRIGHT = (c) Vladimir Pisarev

HEADERS += \
    $$PWD/src/include/callbackproto.h \
    $$PWD/src/include/sp/artif_sign.h


