INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += \
   $$PWD/ldrawini/LDrawIni.h \
   $$PWD/ldrawini/LDrawInP.h \
   $$PWD/dirscan.h \
   $$PWD/f00QuatC.h \
   $$PWD/glext.h \
   $$PWD/glui.h \
   $$PWD/glwinkit.h \
   $$PWD/L3Def.h \
   $$PWD/ldlite.h \
   $$PWD/ldliteVR.h \
   $$PWD/platform.h \
   $$PWD/qbuf.h \
   $$PWD/quant.h \
   $$PWD/StdAfx.h \
   $$PWD/stub.h \
   $$PWD/y.tab.h \

SOURCES += \
   $$PWD/ldrawini/LDrawIni.c \
   $$PWD/camera.c \
   $$PWD/dirscan.c \
   $$PWD/f00QuatC.c \
   $$PWD/gleps.c \
   $$PWD/hoser.c \
   $$PWD/L3Edit.c \
   $$PWD/L3Input.c \
   $$PWD/L3Math.c \
   $$PWD/L3View.c \
   $$PWD/lcolors.c \
   $$PWD/ldglmenu.c \
   $$PWD/ldglpr.c \
   $$PWD/ldliteVR_main.c \
   $$PWD/ldsearch.c \
   $$PWD/lex.yy.c \
   $$PWD/main.c \
   $$PWD/platform.c \
   $$PWD/plugstub.c \
   $$PWD/qbuf.c \
   $$PWD/quant.c \
   $$PWD/stub.c \
   $$PWD/y.tab.c

OTHER_FILES += \
    $$PWD/Info.plist \
    $$PWD/ldglite_w.command \
    $$PWD/doc/ldglite.1 \
    $$PWD/doc/README.TXT \
    $$PWD/set-ldrawdir.command \
    $$PWD/.travis.yml

RC_FILE  += \
    $$PWD/ldglite.rc
