# Created by and for Qt Creator. This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

QT -= core gui
TEMPLATE = app

# The ABI version.
win32: VERSION = 1.3.2.0  # major.minor.patch.build
else: VERSION = 1.3.2     # major.minor.patch

contains(QT_ARCH, x86_64) {
    !macx:ARCH = 64
    macx:ARCH = 32
} else {
    ARCH = 32
}
message("~~~ LDGLITE $$ARCH-bit EXECUTABLE ~~~")

unix:!macx: DEFINES += UNIX
macx: DEFINES += MACOS_X

TARGET +=
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ldrawini

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
        macx {
            # To install libpng follow these instructions:
            # 1. Press Command+Space and type Terminal and press enter/return key.
            # 2. [Optional - if you don't already have Homebrew installed] Run in Terminal app:
            #    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" < /dev/null 2> /dev/null
            #    and press enter/return key. Wait for the command to finish - it may take a long time.
            # 3. Run:
            #    brew install libpng --universal [both 32-bit and 64-bit code]
            # Done! You can now use libpng.
            #
            SYSTEM_PNG_HEADERS = /usr/local/include/png.h
            exists(SYSTEM_PNG_HEADERS) {
                 INCLUDEPATH += /usr/local/include
            } else {
                message("~~~ USING LOCAL COPY PNG HEADERS ~~~")
                INCLUDEPATH += $$PWD/macx/png/include
            }
            SYSTEM_PNG_LIB = /usr/local/lib/libpng.a
            exists(SYSTEM_PNG_LIB) {
                LIBS += /usr/local/lib/libpng.a
            } else {
                message("~~~ USING LOCAL COPY OF PNG LIBRARY ~~~")
                LIBS += ./macx/png/lib/libpng.a
            }
        } else {
            LIBS += -lpng
        }
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

    ENABLE_OFFSCREEN_RENDERING: DEFINES += WIN_DIB_OPTION
    
    DEFINES += USING_FREEGLUT
    DEFINES += FREEGLUT_STATIC
    INCLUDEPATH += \
    $$PWD/win/freeglut/include

    equals (ARCH, 64): LIBS += -L$$_PRO_FILE_PWD_/win/freeglut/lib/x64 -lfreeglut_static
    else: LIBS += -L$$_PRO_FILE_PWD_/win/freeglut/lib -lfreeglut_static
    message("~~~ USING LOCAL COPY OF FREEGLUT ~~~")

    LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32
}

unix:!macx {
    ENABLE_OFFSCREEN_RENDERING: DEFINES += OSMESA_OPTION

    DEFINES += USE_ALPHA_BUFFER

    LIBS += -lOSMesa -lglut -lGLU -lGL -lX11 -lXext -lm
}

macx {
    INCLUDEPATH += \
    $$PWD/macx/include \
    $$PWD/macx/glut/include
    message("~~~ USING LOCAL COPY OF GL HEADERS ~~~")
    
    ENABLE_OFFSCREEN_RENDERING: DEFINES += CGL_OFFSCREEN_OPTION
    
    DEFINES += USING_CARBON
    DEFINES += NEED_MIN_MAX
    DEFINES += NOT_WARPING
    DEFINES += VISIBLE_SPIN_CURSOR
    DEFINES += SAVE_DEPTH_ALL
    DEFINES += SAVE_COLOR_ALL
    DEFINES += MACOS_X_TEST2
    DEFINES += HAVE_STRDUP

    # As we are using Carbon (legacy framework), we can only build i386 for MacOSX - there is no x86_64 port
    MACOSX_TARGET_ARCH = -arch i386
    MACOSX_SDK = -mmacosx-version-min=10.7 -isysroot /Developer/SDKs/MacOSX10.7.sdk
    MACOSX_FRAMEWORKS = -framework OpenGL -framework GLUT -framework Carbon      
    CONFIG += $$MACOSX_FRAMEWORKS $$MACOSX_TARGET_ARCH $$MACOSX_SDK
    
    LIBS += -lobjc -lstdc++ -lm
    
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
    macx: DEFINES += USE_GLUT_MENUS
    INCLUDEPATH += \
    $$PWD/mui/src
    include(mui/mui.pri)
    SOURCES += $$PWD/ldglgui.c
}
