QT      += core
QT      -= opengl
QT      -= gui
CONFIG  -= qt
CONFIG  -= opengl
CONFIG  += thread
CONFIG  += warn_on
CONFIG  += static
CONFIG  += skip_target_version_ext
#win32: CONFIG   += console

DEFINES += QT_THREAD_SUPPORT

contains(QT_ARCH, x86_64) {
  ARCH = 64
} else {
  ARCH = 32
}
DEFINES += ARCH=\\\"$$join(ARCH,,,bit)\\\"

unix: DEFINES += UNIX
macx: DEFINES += MACOS_X

CONFIG(debug, debug|release) {
  DESTDIR = $$join(ARCH,,,bit_debug)
  BUILD = DEBUG
} else {
  DESTDIR = $$join(ARCH,,,bit_release)
  BUILD = RELEASE
}

!contains(CONFIG, ENABLE_PNG): CONFIG += ENABLE_PNG
!contains(CONFIG, ENABLE_TILE_RENDERING): CONFIG += ENABLE_TILE_RENDERING
!contains(CONFIG, ENABLE_OFFSCREEN_RENDERING): CONFIG += ENABLE_OFFSCREEN_RENDERING
!contains(CONFIG, ENABLE_TEST_GUI): CONFIG += ENABLE_TEST_GUI

DEFINES += USE_OPENGL
DEFINES += USE_L3_PARSER
DEFINES += USE_BMP8
DEFINES += HAVE_STRDUP

macx {
  INCLUDEPATH += \
  $$PWD/macx/include \
  $$PWD/macx/glut/include
  QMAKE_CXXFLAGS += -F/System/Library/Frameworks
  # Using local glut headers because source originally written
  # for developer-defined glut library so $$PWD/macx/glut/include above.
  message("~~~ USING LOCAL COPY OF GL HEADERS ~~~")

  ENABLE_OFFSCREEN_RENDERING: DEFINES += CGL_OFFSCREEN_OPTION
  MACOSX_FRAMEWORKS += -framework OpenGL -framework GLUT

  equals(QT_MAJOR_VERSION,4):equals(QT_MINOR_VERSION,7):equals(ARCH,32) {    # qt 4.7 carbon (32bit only)
    DEFINES += USING_CARBON
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    MACOSX_FRAMEWORKS += -framework Carbon
    include(carbon.pri)
    message("~~~ USING CARBON FRAMEWORK ~~~")
  } else {
    DEFINES += USING_COCOA
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    MACOSX_FRAMEWORKS += -framework Cocoa
    message("~~~ USING COCOA FRAMEWORK ~~~")
  }

  DEFINES += NEED_MIN_MAX
  DEFINES += NOT_WARPING
  DEFINES += VISIBLE_SPIN_CURSOR
  DEFINES += SAVE_DEPTH_ALL
  DEFINES += SAVE_COLOR_ALL
  DEFINES += MACOS_X_TEST2

  _LIBS += $$MACOSX_FRAMEWORKS -lobjc -lstdc++ -lm
}

win32 {
  CONFIG += windows

  QMAKE_LFLAGS += -static
  QMAKE_LFLAGS += -static-libgcc
  QMAKE_LFLAGS += -static-libstdc++

  ENABLE_OFFSCREEN_RENDERING: DEFINES += WIN_DIB_OPTION

  DEFINES += USING_FREEGLUT
  DEFINES += FREEGLUT_STATIC
  INCLUDEPATH += \
  $$PWD/win/freeglut/include

  equals (ARCH, 64): _LIBS += -L$$_PRO_FILE_PWD_/win/freeglut/lib/x64 -lfreeglut_static
  else: _LIBS += -L$$_PRO_FILE_PWD_/win/freeglut/lib -lfreeglut_static
  message("~~~ USING LOCAL COPY OF FREEGLUT ~~~")

  _LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32
}

unix:!macx {
  ENABLE_OFFSCREEN_RENDERING: DEFINES += OSMESA_OPTION

  DEFINES += USE_ALPHA_BUFFER

  _LIBS += -lglut -lGLU -lX11 -lXext -lm
}

OBJECTS_DIR = $$DESTDIR/.obj

# suppress warnings
QMAKE_CFLAGS_WARN_ON =  \
                     -Wall -W \
                     -Wno-unused-parameter \
                     -Wno-unused-result \
                     -Wno-implicit-int \
                     -Wno-implicit-fallthrough \
                     -Wno-unused-variable \
                     -Wno-implicit-function-declaration \
                     -Wno-parentheses \
                     -Wno-switch \
                     -Wno-sign-compare \
                     -Wno-incompatible-pointer-types \
                     -Wno-return-type \
                     -Wno-uninitialized \
                     -Wno-format \
                     -Wno-format-security \
                     -Wno-pointer-sign \
                     -Wno-missing-braces \
                     -Wno-unused-function \
                     -Wno-unused-label \
                     -Wno-strict-aliasing \
                     -Wno-format-zero-length \
                     -Wno-format-extra-args \
                     -Wno-unknown-pragmas \
                     -Wno-comment \
                     -Wno-unused-value \
                     -Wno-discarded-qualifiers \
                     -Wno-unused-but-set-variable \
                     -Wno-unused-but-set-parameter
macx {
QMAKE_CFLAGS_WARN_ON +=  \
                     -Wno-macro-redefined \
                     -Wno-deprecated-declarations
}
QMAKE_CXXFLAGS_WARN_ON = $${QMAKE_CFLAGS_WARN_ON}
