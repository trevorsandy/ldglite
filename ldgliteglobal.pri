# qmake Configuration options
# CONFIG+=ENABLE_PNG
# CONFIG+=ENABLE_TILE_RENDERING
# CONFIG+=ENABLE_OFFSCREEN_RENDERING
# CONFIG+=ENABLE_TEST_GUI
# CONFIG+=MAKE_APP_BUNDLE
# CONFIG+=USE_OSMESA_STATIC
# CONFIG+=USE_FREEGLUT_LOCAL # use local freeglut (e.g. Windows, Alma Linux)
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
!exists($${3RD_PREFIX}): message("~~~ ERROR 3rd party repository path not found $$3RD_PREFIX ~~~")

# same more funky stuff to get the local library prefix - all this just to build on OBS' RHEL
OSMESA_ARG = $$find(CONFIG, USE_OSMESA_LOCAL.*)
!isEmpty(OSMESA_ARG) {
    CONFIG -= $$OSMESA_ARG
    CONFIG += $$section(OSMESA_ARG, =, 0, 0)
    isEmpty(OSMESA_LOCAL_PREFIX_): OSMESA_LOCAL_PREFIX_ = $$section(OSMESA_ARG, =, 1, 1)
    !exists($${OSMESA_LOCAL_PREFIX_}): message("~~~ ERROR - Local OSMesa path not found ~~~")
}

USE_OSMESA_STATIC {
  TARGET_VENDOR_VAR = $$(TARGET_VENDOR)
  contains(HOST, Arch):PLATFORM = arch
  else: contains(HOST, Fedora):PLATFORM = fedora
  else:!isEmpty(TARGET_VENDOR_VAR):PLATFORM = $$lower($$TARGET_VENDOR_VAR)
  else: message("~~~ ERROR - PLATFORM not defined ~~~")
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
  CONFIG += console
  CONFIG += USE_FREEGLUT_LOCAL

  !win32-msvc* {
    QMAKE_LFLAGS += -static
    QMAKE_LFLAGS += -static-libgcc
    QMAKE_LFLAGS += -static-libstdc++
  } else {
     QMAKE_LFLAGS += -NODEFAULTLIB:LIBCMT
     QMAKE_CFLAGS_WARN_ON -= -W3
     QMAKE_ADDL_MSVC_FLAGS = -GS -Gd -fp:precise -Zc:forScope
     CONFIG(debug, debug|release) {
       QMAKE_ADDL_MSVC_DEBUG_FLAGS = -RTC1 -Gm $$QMAKE_ADDL_MSVC_FLAGS
       QMAKE_CFLAGS_WARN_ON += -W4 -WX- -wd"4005" -wd"4013" -wd"4018" -wd"4047" -wd"4057" -wd"4068" -wd"4090" -wd"4099" -wd"4100" -wd"4101" -wd"4102" -wd"4113" -wd"4127" -wd"4131" -wd"4133" -wd"4189" -wd"4210" -wd"4244" -wd"4245" -wd"4305" -wd"4431" -wd"4456" -wd"4457" -wd"4458" -wd"4459" -wd"4474" -wd"4477" -wd"4533" -wd"4700" -wd"4701" -wd"4703" -wd"4706" -wd"4706" -wd"4714" -wd"4715" -wd"4716"
       QMAKE_CFLAGS_DEBUG   += $$QMAKE_ADDL_MSVC_DEBUG_FLAGS
       QMAKE_CXXFLAGS_DEBUG += $$QMAKE_ADDL_MSVC_DEBUG_FLAGS
     }
     CONFIG(release, debug|release) {
       QMAKE_ADDL_MSVC_RELEASE_FLAGS = $$QMAKE_ADDL_MSVC_FLAGS -GF -Gy
       QMAKE_CFLAGS_OPTIMIZE += -Ob1 -Oi -Ot
       QMAKE_CFLAGS_WARN_ON  += -W1 -WX- -wd"4005" -wd"4013" -wd"4018" -wd"4047" -wd"4057" -wd"4068" -wd"4090" -wd"4099" -wd"4100" -wd"4101" -wd"4102" -wd"4113" -wd"4127" -wd"4131" -wd"4133" -wd"4189" -wd"4210" -wd"4244" -wd"4245" -wd"4305" -wd"4431" -wd"4456" -wd"4457" -wd"4458" -wd"4459" -wd"4474" -wd"4477" -wd"4533" -wd"4700" -wd"4701" -wd"4703" -wd"4706" -wd"4706" -wd"4714" -wd"4715" -wd"4716"
       QMAKE_CFLAGS_RELEASE  += $$QMAKE_ADDL_MSVC_RELEASE_FLAGS
       QMAKE_CXXFLAGS_RELEASE += $$QMAKE_ADDL_MSVC_RELEASE_FLAGS
     }
     QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON
  }

  ENABLE_OFFSCREEN_RENDERING: DEFINES += WIN_DIB_OPTION

  DEFINES += USING_FREEGLUT
  DEFINES += FREEGLUT_STATIC

  win32-msvc*: {
  DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE=1 _CRT_NONSTDC_NO_WARNINGS=1
  }

  INCLUDEPATH += \
  $$PWD/win/freeglut/include

  equals (ARCH, 64): _LIBS += -L$$PWD/win/freeglut/lib/x64 -lfreeglut_static
  else:              _LIBS += -L$$PWD/win/freeglut/lib -lfreeglut_static

  _LIBS += -lshell32 -lglu32 -lopengl32 -lwinmm -lgdi32 -lcomdlg32 -lole32
} else {
  QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unknown-pragmas
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

  INCLUDEPATH += $${SYS_LIBINC_}

  USE_FREEGLUT_LOCAL {
    INCLUDEPATH += $$PWD/linux/freeglut/include
    contains(BUILD_ARCH, aarch64) {
      FREEGLUT_LIBDIR = -L$$PWD/linux/freeglut/lib/aarch64
    } else {
      FREEGLUT_LIBDIR = -L$$PWD/linux/freeglut/lib
    }
    FREEGLUT_LIBS  = -lXxf86vm -lXrandr -lXi
  }

  DEFINES += USE_ALPHA_BUFFER

  ENABLE_OFFSCREEN_RENDERING {

    DEFINES += OSMESA_OPTION

    # OSMesa with Gallium support - static library built from source
    USE_OSMESA_STATIC {
      OSMESA_INC           = $$system($${3RD_PREFIX}/mesa/$${PLATFORM}/osmesa-config --cflags)
      INCLUDEPATH         += $${OSMESA_INC}
      isEmpty(OSMESA_INC): message("~~~ OSMESA - ERROR OSMesa include path not found ~~~")
      OSMESA_LIBS          = $$system($${3RD_PREFIX}/mesa/$${PLATFORM}/osmesa-config --libs)
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

      OSMESA_LDFLAGS = $$system($${3RD_PREFIX}/mesa/$${PLATFORM}/osmesa-config --ldflags)
      isEmpty(OSMESA_LDFLAGS): \
      message("~~~ OSMESA - ERROR OSMesa link flags not defined ~~~") \
      else: \
      _LIBS         += $${OSMESA_LDFLAGS}
      !isEmpty(FREEGLUT_LIBDIR): \
      _LIBS         += $${FREEGLUT_LIBDIR}
      _LIBS         += -lOSMesa -lGLU -lglut -lGL -lX11 -lXext
      !isEmpty(FREEGLUT_LIBS): \
      _LIBS         += $${FREEGLUT_LIBS}
      _LIBS         += -lm

    } # USE_OSMESA_STATIC
    else
    {
      USE_OSMESA_LOCAL {
        INCLUDEPATH   += $${OSMESA_LOCAL_PREFIX_}/include
        OSMESA_LIBDIR  = -L$${OSMESA_LOCAL_PREFIX_}/lib$${LIB_ARCH}
        _LIBS         += $${OSMESA_LIBDIR}
      }

      # OSMesa (OffScreen) - system, dynamic libraries and static local freeglut
      !isEmpty(FREEGLUT_LIBDIR): \
      _LIBS         += $${FREEGLUT_LIBDIR}
      _LIBS         += -lOSMesa -lGLU -lglut -lGL -lX11 -lXext
      !isEmpty(FREEGLUT_LIBS): \
      _LIBS         += $${FREEGLUT_LIBS}
      _LIBS         += -lm
    }

  } # ENABLE_OFFSCREEN_RENDERING
  else
  {
    # Mesa (OnScreen) - OpenGL
    !isEmpty(FREEGLUT_LIBDIR): \
    _LIBS         += $${FREEGLUT_LIBDIR}
    _LIBS         += -lGL -lGLU -lglut -lX11 -lXext
    _LIBS         += $${FREEGLUT_LIBS}
    _LIBS         += -lm
  }
}

PRECOMPILED_DIR = $$DESTDIR/.pch
OBJECTS_DIR     = $$DESTDIR/.obj
MOC_DIR         = $$DESTDIR/.moc
RCC_DIR         = $$DESTDIR/.qrc
UI_DIR          = $$DESTDIR/.ui

# suppress warnings
!win32-msvc* {
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
}
macx {
QMAKE_CFLAGS_WARN_ON +=  \
                     -Wno-macro-redefined \
                     -Wno-deprecated-declarations \
                     -Wno-absolute-value
} else {
!win32-msvc* {
QMAKE_CFLAGS_WARN_ON +=  \
                     -Wno-discarded-qualifiers \
                     -Wno-unused-but-set-variable \
                     -Wno-unused-but-set-parameter
}
QMAKE_CXXFLAGS_WARN_ON = $${QMAKE_CFLAGS_WARN_ON}
}
