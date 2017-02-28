# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.
QT -= core gui
TEMPLATE = app
# The ABI version.
win32: VERSION = 1.3.1.0  # major.minor.patch.build
else: VERSION = 1.3.1     # major.minor.patch

contains(QT_ARCH, x86_64) {
    ARCH = 64
} else {
    ARCH = 32
}

TARGET +=
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ./ldrawini

LIBS += -lopengl32
CONFIG += static
LIBS += -static

!contains(CONFIG, ENABLE_PNG): CONFIG += ENABLE_PNG
!contains(CONFIG, ENABLE_TILE_RENDERING): CONFIG += ENABLE_TILE_RENDERING
!contains(CONFIG, ENABLE_OFFSCREEN_RENDERING): CONFIG += ENABLE_OFFSCREEN_RENDERING
!contains(CONFIG, ENABLE_TEST_GUI): CONFIG += ENABLE_TEST_GUI

DEFINES += USE_OPENGL
DEFINES += USE_L3_PARSER
DEFINES += USE_BMP8

ENABLE_PNG {
    DEFINES += USE_PNG
    INCLUDEPATH += \
    $$PWD/png/include
    win32 {
        equals (ARCH, 64): LIBS += -L$$_PRO_FILE_PWD_/png/lib/x64 -lpng
        else: LIBS += -L$$_PRO_FILE_PWD_/png/lib -lpng
    } else {
        LIBS += -L$$_PRO_FILE_PWD_/png/lib -lpng
    }
    LIBS +=  -lz
}

win32 {
    CONFIG += windows
    CONFIG += debug_and_release

    QMAKE_TARGET_COMPANY = "Don Heyse"
    QMAKE_TARGET_DESCRIPTION = "LDraw Image Renderer"
    QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2016  Don Heyse"
    QMAKE_TARGET_PRODUCT = "LDGLite ($$ARCH-bit)"

    DEFINES += USING_FREEGLUT
    DEFINES += FREEGLUT_STATIC
    INCLUDEPATH += \
    $$PWD/freeglut/include

    RC_FILE = ldglite.rc

    equals (ARCH, 64): LIBS += -L$$_PRO_FILE_PWD_/freeglut/lib/x64 -lfreeglut_static
    else: LIBS += -L$$_PRO_FILE_PWD_/freeglut/lib -lfreeglut_static

    ENABLE_OFFSCREEN_RENDERING: DEFINES += WIN_DIB_OPTION
    LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32
}
unix:!macx {
    ENABLE_OFFSCREEN_RENDERING: DEFINES += OSMESA_OPTION
    LIBS += -lOSMesa -lglut -lGLU -lGL -lX11 -lXext -lm
}

CONFIG += skip_target_version_ext
TARGET = ldglite
CONFIG(debug, debug|release) {
        DESTDIR = debug
} else {
        DESTDIR = release
}

OBJECTS_DIR = $$DESTDIR/.obj
RCC_DIR = $$DESTDIR/.qrc

ENABLE_TEST_GUI {
    DEFINES += TEST_MUI_GUI
    INCLUDEPATH += \
    $$PWD/mui/src
    include(mui/mui.pri)
}
ENABLE_TILE_RENDERING {
    DEFINES += TILE_RENDER_OPTION
    HEADERS += $$PWD/tr.h
    SOURCES += $$PWD/tr.c
}

HEADERS += \
   $$PWD/ldrawini/LDrawIni.h \
   $$PWD/ldrawini/LDrawInP.h \
   $$PWD/dirscan.h \
   $$PWD/f00QuatC.h \
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
   $$PWD/StdAfx.cpp \
   $$PWD/stub.c \
   $$PWD/y.tab.c
