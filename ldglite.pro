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
unix:!macx: DEFINES += UNIX
macx: DEFINES += MACOS_X

message("~~~ LDGLITE $$ARCH-bit EXECUTABLE ~~~")
TARGET +=
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ./ldrawini

!contains(CONFIG, ENABLE_PNG): CONFIG += ENABLE_PNG
!contains(CONFIG, ENABLE_TILE_RENDERING): CONFIG += ENABLE_TILE_RENDERING
!contains(CONFIG, ENABLE_OFFSCREEN_RENDERING): CONFIG += ENABLE_OFFSCREEN_RENDERING
!contains(CONFIG, ENABLE_TEST_GUI): CONFIG += ENABLE_TEST_GUI

DEFINES += USE_OPENGL
DEFINES += USE_L3_PARSER
DEFINES += USE_BMP8
DEFINES += HAVE_STRDUP

include($$PWD/ldglite.pri)

ENABLE_PNG {
    DEFINES += USE_PNG
    INCLUDEPATH += \
    $$PWD/win/png/include
    win32 {
        equals (ARCH, 64): LIBS += -L$$_PRO_FILE_PWD_/win/png/lib/x64 -lpng
        else: LIBS += -L$$_PRO_FILE_PWD_/win/png/lib -lpng
        message("~~~ USING LOCAL COPY OF FREEGLUT ~~~")
    } else {
        LIBS += -lpng
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
    $$PWD/win/freeglut/include

    equals (ARCH, 64): LIBS += -L$$_PRO_FILE_PWD_/win/freeglut/lib/x64 -lfreeglut_static
    else: LIBS += -L$$_PRO_FILE_PWD_/win/freeglut/lib -lfreeglut_static
    message("~~~ USING LOCAL COPY OF FREEGLUT ~~~")

    ENABLE_OFFSCREEN_RENDERING: DEFINES += WIN_DIB_OPTION
    LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32
}

unix:!macx {
    DEFINES += USE_ALPHA_BUFFER

    ENABLE_OFFSCREEN_RENDERING: DEFINES += OSMESA_OPTION
    LIBS += -lOSMesa -lglut -lGLU -lGL -lX11 -lXext -lm
}

macx {

    INCLUDEPATH += \
    $$PWD/macx/include
    message("~~~ USING LOCAL COPY OF GL HEADERS ~~~")

    MAKE_LDGLITE_BUNDLE_TARGET = $$PWD/make-ldglite-bundle.sh
    MAKE_LDGLITE_BUNDLE_COMMAND = $$MAKE_LDGLITE_BUNDLE_TARGET $$VERSION
    CHMOD_COMMAND = chmod 755 $$MAKE_LDGLITE_BUNDLE_TARGET
    QMAKE_POST_LINK += $$escape_expand(\n\t)  \
                       $$shell_quote$${CHMOD_COMMAND} \
                       $$escape_expand(\n\t)  \
                       $$shell_quote$${MAKE_LDGLITE_BUNDLE_COMMAND}
}

CONFIG += skip_target_version_ext
TARGET = ldglite
CONFIG(debug, debug|release) {
    message("~~~ LDGLITE DEBUG BUILD ~~~")
    DESTDIR = debug
} else {
    DESTDIR = release
    message("~~~ LDGLITE RELEASE BUILD ~~~")
}

OBJECTS_DIR = $$DESTDIR/.obj

ENABLE_TILE_RENDERING {
    DEFINES += TILE_RENDER_OPTION
    HEADERS += $$PWD/tr.h
    SOURCES += $$PWD/tr.c
}

ENABLE_TEST_GUI {
    DEFINES += TEST_MUI_GUI
    INCLUDEPATH += \
    $$PWD/mui/src
    include(mui/mui.pri)
    SOURCES += $$PWD/ldglgui.c
}
