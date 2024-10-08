INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += \
   $$PWD/dirscan.h \
   $$PWD/f00QuatC.h \
   $$PWD/functionheaders.h \
   $$PWD/getargv.h \
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
   $$PWD/tinyfiledialogs.h \
   $$PWD/wglext.h \
   $$PWD/win32_dirent.h \
   $$PWD/wstubs.h \
   $$PWD/y.tab.h \

SOURCES += \
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
   $$PWD/ldglgui.c \
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
   $$PWD/tinyfiledialogs.c \
   $$PWD/y.tab.c

OTHER_FILES += \
   $$PWD/Info.plist \
   $$PWD/ldglite_w.command \
   $$PWD/set-ldrawdir.command \
   $$PWD/../.gitignore \
   $$PWD/../.github/workflows/build.yml \
   $$PWD/../.github/ISSUE_TEMPLATE/bug_report.md \
   $$PWD/../.github/ISSUE_TEMPLATE/feature_request.md \
   $$PWD/../utils/install-dev-packages.sh
