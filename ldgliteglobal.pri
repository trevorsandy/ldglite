# qmake Configuration options
# CONFIG+=ENABLE_PNG
# CONFIG+=ENABLE_TILE_RENDERING
# CONFIG+=ENABLE_OFFSCREEN_RENDERING
# CONFIG+=ENABLE_TEST_GUI
# CONFIG+=MAKE_APP_BUNDLE
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_linux_3rdparty
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_macos_3rdparty
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_windows_3rdparty

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

win32:HOST = $$system(systeminfo | findstr /B /C:\"OS Name\")
unix:!macx:HOST = $$system(. /etc/os-release && if test \"$PRETTY_NAME\" != \"\"; then echo \"$PRETTY_NAME\"; else echo `uname`; fi)
macx:HOST = $$system(echo `sw_vers -productName` `sw_vers -productVersion`)

# some funky processing to get the prefix passed in on the command line
3RD_ARG = $$find(CONFIG, 3RD_PARTY_INSTALL.*)
!isEmpty(3RD_ARG): CONFIG -= $$3RD_ARG
CONFIG += $$section(3RD_ARG, =, 0, 0)
isEmpty(3RD_PREFIX):3RD_PREFIX = $$_PRO_FILE_PWD_/$$section(3RD_ARG, =, 1, 1)
!exists($${3RD_PREFIX}): message("~~~ ERROR 3rd party repository path not found ~~~")

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

  equals (ARCH, 64): _LIBS += -L$$PWD/win/freeglut/lib/x64 -lfreeglut_static
  else:              _LIBS += -L$$PWD/win/freeglut/lib -lfreeglut_static
  message("~~~ USING LOCAL COPY OF FREEGLUT ~~~")

  _LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32
}

unix:!macx {
  # detect system libraries paths
  SYSTEM_PREFIX_     = /usr
  SYS_LIBINC_        = $${SYSTEM_PREFIX_}/include
  exists($${SYSTEM_PREFIX_}/lib/$$QT_ARCH-linux-gnu) {             # Debian
     SYS_LIBDIR_     = $${SYSTEM_PREFIX_}/lib/$$QT_ARCH-linux-gnu
  } else: exists($${SYSTEM_PREFIX_}/lib$$ARCH/) {                  # RedHat (64bit)
     SYS_LIBDIR_     = $${SYSTEM_PREFIX_}/lib$$ARCH
  } else {                                                         # Arch, RedHat (32bit)
     SYS_LIBDIR_     = $${SYSTEM_PREFIX_}/lib
  }
  INCLUDEPATH +=  $${SYS_LIBINC_}

  DEFINES += USE_ALPHA_BUFFER

  ENABLE_OFFSCREEN_RENDERING {

    DEFINES += OSMESA_OPTION

    # OSMesa with Gallium support - static library built from source
    contains(HOST, Fedora) {
      OSMESA_INC           = $$system($${3RD_PREFIX}/mesa/osmesa-config --cflags)
      isEmpty(OSMESA_INC): message("~~~ OSMESA - ERROR OSMesa include path not found ~~~")
      else: INCLUDEPATH   += $${OSMESA_INC}
      OSMESA_LIBS          = $$system($${3RD_PREFIX}/mesa/osmesa-config --libs)
      isEmpty(OSMESA_LIBS): message("~~~ OSMESA - ERROR OSMesa libraries not defined ~~~")
      else: _LIBS         += $${OSMESA_LIBS} -lglut -lX11 -lXext

      exists (/usr/bin/llvm-config) {
          message("~~~ LLVM - llvm-config found ~~~")
          LLVM_LIB_PATH    = $${SYS_LIBDIR_}
          isEmpty(LLVM_LIB_PATH): message("~~~ LLVM - ERROR llvm library path not found ~~~")
          else: LLVM_LIBS  = -L$${LLVM_LIB_PATH}
          LLVM_LIB_NAME    = $$system(/usr/bin/llvm-config --libs engine mcjit)
          isEmpty(LLVM_LIBS): message("~~~ LLVM - ERROR llvm library not found ~~~")
          else: LLVM_LIBS += $${LLVM_LIB_NAME}
          LLVM_SYS_LIBS    = $$system(/usr/bin/llvm-config --system-libs)
          isEmpty(LLVM_SYS_LIBS): message("~~~ LLVM - NOTICE llvm system libs not defined ~~~")
          else: LLVM_LIBS += $${LLVM_SYS_LIBS}
          LLVM_LDFLAGS     = $$system(/usr/bin/llvm-config --ldflags)
          isEmpty(LLVM_LDFLAGS): message("~~~ LLVM - WARNIGN llvm ldflags not found ~~~")
          else: LLVM_LIBS += $${LLVM_LDFLAGS}

          _LIBS     += $${LLVM_LIBS}
      } else {
        message("~~~ LLVM - ERROR llvm-config not found ~~~")
      }

      OSMESA_LDFLAGS    = $$system($${3RD_PREFIX}/mesa/osmesa-config --ldflags)
      isEmpty(OSMESA_LDFLAGS): message("~~~ OSMESA - ERROR OSMesa link flags not defined ~~~")
      else: _LIBS += $${OSMESA_LDFLAGS}

    } else {
      # OSMesa - system dynamic library
      _LIBS += -lOSMesa -lGLU -lglut -lX11 -lXext -lm
    }
  } else {
    # Mesa - OpenGL
    _LIBS += -lGL -lGLU -lglut -lX11 -lXext -lm
  }
}

PRECOMPILED_DIR = $$DESTDIR/.pch
OBJECTS_DIR     = $$DESTDIR/.obj
MOC_DIR         = $$DESTDIR/.moc
RCC_DIR         = $$DESTDIR/.qrc
UI_DIR          = $$DESTDIR/.ui

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
                     -Wno-unused-value
macx {
QMAKE_CFLAGS_WARN_ON +=  \
                     -Wno-macro-redefined \
                     -Wno-deprecated-declarations \
                     -Wno-absolute-value
} else {
QMAKE_CFLAGS_WARN_ON +=  \
                     -Wno-discarded-qualifiers \
                     -Wno-unused-but-set-variable \
                     -Wno-unused-but-set-parameter
}
QMAKE_CXXFLAGS_WARN_ON = $${QMAKE_CFLAGS_WARN_ON}
