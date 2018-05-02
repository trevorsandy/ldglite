# qmake Configuration options
# CONFIG+=ENABLE_PNG
# CONFIG+=ENABLE_TILE_RENDERING
# CONFIG+=ENABLE_OFFSCREEN_RENDERING
# CONFIG+=ENABLE_TEST_GUI
# CONFIG+=MAKE_APP_BUNDLE
# CONFIG+=USE_OSMESA_STATIC
# CONFIG+=USE_OSMESA_LOCAL   # use local OSmesa and LLVM libraries - for OBS images w/o OSMesa stuff (e.g. RHEL)
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

DEFINES += QT_THREAD_SUPPORT

win32:HOST = $$system(systeminfo | findstr /B /C:\"OS Name\")
unix:!macx:HOST = $$system(. /etc/os-release 2>/dev/null; [ -n \"$PRETTY_NAME\" ] && echo \"$PRETTY_NAME\" || echo `uname`)
macx:HOST = $$system(echo `sw_vers -productName` `sw_vers -productVersion`)
isEmpty(HOST):HOST = UNKNOWN HOST

# platform switch
BUILD_ARCH = $$(TARGET_CPU)
if (contains(QT_ARCH, x86_64)|contains(QT_ARCH, arm64)|contains(BUILD_ARCH, aarch64)) {
  ARCH     = 64
  LIB_ARCH = 64
} else {
  ARCH     = 32
  LIB_ARCH =
}

DEFINES += ARCH=\\\"$$join(ARCH,,,bit)\\\"

unix: DEFINES += UNIX
macx: DEFINES += MACOS_X

QMAKE_CXXFLAGS  += $(Q_CXXFLAGS)
QMAKE_CFLAGS    += $(Q_CFLAGS)
QMAKE_LFLAGS    += $(Q_LDFLAGS)

CONFIG(debug, debug|release) {
  DESTDIR = $$join(ARCH,,,bit_debug)
  BUILD = DEBUG
} else {
  DESTDIR = $$join(ARCH,,,bit_release)
  BUILD = RELEASE
}

# some funky processing to get the prefix passed in on the command line
3RD_ARG = $$find(CONFIG, 3RD_PARTY_INSTALL.*)
!isEmpty(3RD_ARG): CONFIG -= $$3RD_ARG
CONFIG += $$section(3RD_ARG, =, 0, 0)
isEmpty(3RD_PREFIX):3RD_PREFIX = $$_PRO_FILE_PWD_/$$section(3RD_ARG, =, 1, 1)
!exists($${3RD_PREFIX}): message("~~~ ERROR 3rd party repository path not found ~~~")

# same more funky stuff to get the local library prefix - all this just to build on OBS' RHEL
OSMESA_ARG = $$find(CONFIG, USE_OSMESA_LOCAL.*)
!isEmpty(OSMESA_ARG) {
    CONFIG -= $$OSMESA_ARG
    CONFIG += $$section(OSMESA_ARG, =, 0, 0)
    isEmpty(OSMESA_LOCAL_PREFIX_): OSMESA_LOCAL_PREFIX_ = $$section(OSMESA_ARG, =, 1, 1)
    !exists($${OSMESA_LOCAL_PREFIX_}): message("~~~ ERROR - Local OSMesa path not found ~~~")
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

  equals (ARCH, 64): _LIBS += -L$$PWD/win/freeglut/lib/x64 -lfreeglut_static
  else:              _LIBS += -L$$PWD/win/freeglut/lib -lfreeglut_static

  _LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32 -lcomdlg32 -lole32
}

unix:!macx {
  # detect system libraries paths
  SYSTEM_PREFIX_      = /usr
  SYS_LIBINC_         = $${SYSTEM_PREFIX_}/include
  exists($${SYSTEM_PREFIX_}/lib/$$QT_ARCH-linux-gnu) {               # Debian
      SYS_LIBDIR_     = $${SYSTEM_PREFIX_}/lib/$$QT_ARCH-linux-gnu
  } else: exists($${SYSTEM_PREFIX_}/lib$${LIB_ARCH}) {               # RedHat, Arch - lIB_ARCH is empyt for 32bit
      SYS_LIBDIR_     = $${SYSTEM_PREFIX_}/lib$${LIB_ARCH}
  } else {                                                           # Arch - acutally should never get here
      SYS_LIBDIR_     = $${SYSTEM_PREFIX_}/lib
  }

  INCLUDEPATH +=  $${SYS_LIBINC_}

  DEFINES += USE_ALPHA_BUFFER

  ENABLE_OFFSCREEN_RENDERING {

    DEFINES += OSMESA_OPTION

    # OSMesa with Gallium support - static library built from source
    USE_OSMESA_STATIC {
      OSMESA_INC           = $$system($${3RD_PREFIX}/mesa/osmesa-config --cflags)
      INCLUDEPATH         += $${OSMESA_INC}
      isEmpty(OSMESA_INC): message("~~~ OSMESA - ERROR OSMesa include path not found ~~~")
      OSMESA_LIBS          = $$system($${3RD_PREFIX}/mesa/osmesa-config --libs)
      isEmpty(OSMESA_LIBS): message("~~~ OSMESA - ERROR OSMesa libraries not defined ~~~")
      _LIBS               += $${OSMESA_LIBS} -lglut -lX11 -lXext

      NO_GALLIUM {
        message("~~~ LLVM not needed - Gallium driver not used ~~~")
      } else {
        isEmpty(LLVM_PREFIX_): LLVM_PREFIX_ = $${SYSTEM_PREFIX_}
        exists($${LLVM_PREFIX_}/bin/llvm-config) {
          LLVM_LDFLAGS     = $$system($${LLVM_PREFIX_}/bin/llvm-config --ldflags)
          LLVM_LIBS       += $${LLVM_LDFLAGS}
          isEmpty(LLVM_LDFLAGS): message("~~~ LLVM - ERROR llvm ldflags not found ~~~")
          LLVM_LIB_NAME    = $$system($${LLVM_PREFIX_}/bin/llvm-config --libs engine mcjit)
          LLVM_LIBS       += $${LLVM_LIB_NAME}
          isEmpty(LLVM_LIBS): message("~~~ LLVM - ERROR llvm library not found ~~~")
          _LIBS           += $${LLVM_LIBS}
        } else {
          message("~~~ LLVM - ERROR llvm-config not found ~~~")
        }
      }

      OSMESA_LDFLAGS   = $$system($${3RD_PREFIX}/mesa/osmesa-config --ldflags)
      isEmpty(OSMESA_LDFLAGS): message("~~~ OSMESA - ERROR OSMesa link flags not defined ~~~")
      _LIBS           += $${OSMESA_LDFLAGS}

    } else {

      USE_OSMESA_LOCAL {
        INCLUDEPATH    += $${OSMESA_LOCAL_PREFIX_}/include
        OSMESA_LIBDIR   = -L$${OSMESA_LOCAL_PREFIX_}/lib$${LIB_ARCH}
      }

      # For some reason SLE 15 on SUSE OBS does not have freeglut.
      SLE_VER = $$system(echo $$(PLATFORM_PRETTY_OBS))
      contains(SLE_VER, Enterprise):contains(SLE_VER, 150000) {
        INCLUDEPATH += \
        $$PWD/linux/sle15/freeglut/include
        SLE_LIBDIR = -L$$PWD/linux/sle15/freeglut/lib
        SLE_LIBS = -lXxf86vm -lXrandr -lXi
      }
      # OSMesa (OffScreen) - system dynamic libraries
      #_LIBS += $${OSMESA_LIBDIR} -lOSMesa -lGLU -lglut -lX11 -lXext -lm

      # OSMesa (OffScreen) - system, dynamic libraries and static local freeglut
      _LIBS += $${OSMESA_LIBDIR} $${SLE_LIBDIR} -lOSMesa -lGLU -lglut -lGL -lX11 -lXext $${SLE_LIBS} -lm
    }

  } else {
    # Mesa (OnScreen) - OpenGL
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
