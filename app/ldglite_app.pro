TEMPLATE       = app
TARGET         = LDGLite
QT            += core
QT            -= opengl
QT            -= gui
CONFIG        -= qt
CONFIG        -= opengl
CONFIG        += thread
CONFIG        += static
CONFIG        += warn_on
CONFIG        += skip_target_version_ext
win32: CONFIG += console
macx:  CONFIG -= app_bundle # do not bundle macOS app

include($$PWD/../ldgliteglobal.pri)

# The ABI version.
VER_MAJ = 1
VER_MIN = 3
VER_PAT = 8
VER_BLD = 0

win32 {
  VERSION = $$VER_MAJ"."$$VER_MIN"."$$VER_PAT"."$$VER_BLD # major.minor.patch.build
  QMAKE_TARGET_COMPANY = "Don Heyse"
  QMAKE_TARGET_DESCRIPTION = "LDraw Image Renderer"
  QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2017 - 2025 Trevor SANDY, Don Heyse"
  QMAKE_TARGET_PRODUCT = "$${TARGET} ($$join(ARCH,,,bit))"
  RC_LANG  = "English (United Kingdom)"
  RC_ICONS = "ldglite.ico"
} else {
  VERSION = $$VER_MAJ"."$$VER_MIN"."$$VER_PAT              # major.minor.patch
}
DEFINES += VERSION_INFO=\\\"$$VERSION\\\"

unix|msys:!macx: \
TARGET = ldglite

DEPENDPATH  += .
INCLUDEPATH += . ../ldrawini
ENABLE_TEST_GUI: \
INCLUDEPATH += ../mui
win32-msvc*: \
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

# messages
message("~~~ $$upper($$TARGET) $$upper($$BUILD_ARCH) $${BUILD} ON $$upper($$HOST) ~~~")
USE_FREEGLUT_LOCAL: message("~~~ USING LOCAL STATIC FREEGLUT LIBRARY ~~~")
!isEmpty(OSMESA_LIBDIR): message("~~~ OSMESA - USING LOCAL LIBRARIES AT $${OSMESA_LOCAL_PREFIX_}/lib$$LIB_ARCH ~~~")
else:USE_OSMESA_STATIC: message("~~~ NOTICE: USING OSMESA BUILT FROM SOURCE LIBRARY ~~~")
else:!win32: message("~~~ NOTICE: USING OSMESA SYSTEM LIBRARY")

ENABLE_TILE_RENDERING {
  DEFINES += TILE_RENDER_OPTION
  include($$PWD/tiles.pri)
}

ENABLE_PNG {
  DEFINES += USE_PNG
  win32-msvc* {
    message("~~~ USING LOCAL COPY OF PNG AND Z LIBRARIES ~~~")

    INCLUDEPATH += \
    $$PWD/../win/png/include \
    $$PWD/../win/zlib/include

    BUILD_WORKER_VERSION = $$(LP3D_VSVERSION)
    isEmpty(BUILD_WORKER_VERSION): BUILD_WORKER_VERSION = 2019
    message("~~~ Build worker: Visual Studio $$BUILD_WORKER_VERSION ~~~")
    equals(BUILD_WORKER_VERSION, 2019) | greaterThan(BUILD_WORKER_VERSION, 2019) {
        contains(QT_ARCH,i386): VSVER=vs2017
        else: VSVER=vs2019
    } else {
        VSVER=vs2015
    }
    message("~~~ $$upper($$QT_ARCH) MSVS library version: $$VSVER ~~~")

    equals (ARCH, 64) {
        LIBS_ += -L$$_PRO_FILE_PWD_/../win/png/lib/x64 -llibpng16-$${VSVER}
        LIBS_ += -L$$_PRO_FILE_PWD_/../win/zlib/x64 -lzlib-$${VSVER}
    } else {
        LIBS_ += -L$$_PRO_FILE_PWD_/../win/png/lib -llibpng16-$${VSVER}
        LIBS_ += -L$$_PRO_FILE_PWD_/../win/zlib -lzlib-$${VSVER}
    }

  } else:macx {
    contains(QT_ARCH,arm64) {
        SYSTEM_PNG_HEADERS = /opt/homebrew/include
        SYSTEM_PNG_LIB = /opt/homebrew/lib/libpng.a
    } else {
        SYSTEM_PNG_HEADERS = /usr/local/include
        SYSTEM_PNG_LIB = /usr/local/lib/libpng.a
    }

    LOCAL_LIB_PREFIX = $$absolute_path( $$_PRO_FILE_PWD_/../macx )

    exists($${SYSTEM_PNG_HEADERS}/png.h) {
      message("~~~ USING SYSTEM PNG HEADERS $${SYSTEM_PNG_HEADERS} ~~~")
      INCLUDEPATH += $${SYSTEM_PNG_HEADERS}
    } else {
      message("~~~ USING LOCAL COPY OF PNG HEADERS ~~~")
      INCLUDEPATH += $${LOCAL_LIB_PREFIX}/png/include
    }

    exists($${SYSTEM_PNG_LIB}) {
      message("~~~ USING SYSTEM PNG LIBRARY $${SYSTEM_PNG_LIB} ~~~")
      LIBS_ += $${SYSTEM_PNG_LIB}
    } else {
      message("~~~ USING LOCAL COPY OF PNG LIBRARY ~~~")
      contains(QT_ARCH,x86_64): \
      LIBS_ += $${LOCAL_LIB_PREFIX}/x86_64/png/lib/libpng.a
      else:message("~~~ ERROR: NO LOCAL COPY OF $$upper($$QT_ARCH) PNG LIBRARY AVAILABLE ~~~")
    }
  } else {
    LIBS_ += -lpng
  }
}

LIBS_ += -L../ldrawini/$$DESTDIR -lldrawini

ENABLE_TEST_GUI: \
LIBS_ += -L../mui/$$DESTDIR -lmui

LIBS  += $${LIBS_} $${_LIBS}

!win32-msvc*: \
LIBS  += -lz
#message("~~~ DEBUG_LIBS: $$LIBS ~~~")
#message("~~~ DEBUG_CONFIG: $$CONFIG ~~~")

include($$PWD/ldgliteapp.pri)

macx {
  MAKE_APP_BUNDLE {
    ICON = ldglite.icns
    QMAKE_INFO_PLIST = $$_PRO_FILE_PWD_/Info.plist

    ldglite_osxwrapper.files  += ldglite_w.command
    ldglite_osxwrapper.path    = Contents/MacOS

    ldglite_docs.files        += $$_PRO_FILE_PWD_/../doc/LICENCE $$_PRO_FILE_PWD_/../doc/Readme.macLdGLite $$_PRO_FILE_PWD_/../doc/README.TXT
    ldglite_docs.path          = Contents/doc

    set_ldraw_directory.files += set-ldrawdir.command
    set_ldraw_directory.path   = Contents/Resources

    QMAKE_BUNDLE_DATA += ldglite_osxwrapper set_ldraw_directory

    INFO_PLIST_FILE  = $$shell_quote$$DESTDIR/$${TARGET}.app/Contents/Info.plist
    PLIST_COMMAND    = /usr/libexec/PlistBuddy -c
    TYPEINFO_COMMAND = /bin/echo "APPLLdGL" > $$DESTDIR/$${TARGET}.app/Contents/PkgInfo
    WRAPPER_TARGET   = $$DESTDIR/$${TARGET}.app/Contents/MacOS/ldglite_w.command
    WRAPPER_CHMOD_COMMAND  = chmod 755 $$WRAPPER_TARGET
    LDRAWDIR_TARGET  = $$DESTDIR/$${TARGET}.app/Contents/Resources/set-ldrawdir.command
    LDRAWDIR_CHMOD_COMMAND = chmod 755 $$LDRAWDIR_TARGET
    QMAKE_POST_LINK += $$escape_expand(\n\t)   \
                       $$PLIST_COMMAND \"Set :CFBundleShortVersionString $${VERSION}\" $${INFO_PLIST_FILE}  \
                       $$escape_expand(\n\t)   \
                       $$PLIST_COMMAND \"Set :CFBundleVersion $${VERSION}\" $${INFO_PLIST_FILE} \
                       $$escape_expand(\n\t)   \
                       $$PLIST_COMMAND \"Set :CFBundleGetInfoString $${TARGET} $${VERSION} https://github.com/trevorsandy/ldglite\" $${INFO_PLIST_FILE} \
                       $$escape_expand(\n\t)   \
                       $$shell_quote$${TYPEINFO_COMMAND} \
                       $$escape_expand(\n\t)   \
                       $$shell_quote$${WRAPPER_CHMOD_COMMAND} \
                       $$escape_expand(\n\t)   \
                       $$shell_quote$${LDRAWDIR_CHMOD_COMMAND}
  } else {
    CONFIG     -= app_bundle   # don't creatre app bundle
    CONFIG     += sdk_no_version_check
  }
}

3RD_PARTY_INSTALL {
  isEmpty(3RD_PACKAGE_VER):3RD_PACKAGE_VER = $$TARGET-$$VER_MAJ"."$$VER_MIN
  isEmpty(3RD_BINDIR):3RD_BINDIR           = $$3RD_PREFIX/$$3RD_PACKAGE_VER/bin/$$QT_ARCH
  isEmpty(3RD_DOCDIR):3RD_DOCDIR           = $$3RD_PREFIX/$$3RD_PACKAGE_VER/docs
  isEmpty(3RD_RESOURCES):3RD_RESOURCES     = $$3RD_PREFIX/$$3RD_PACKAGE_VER/resources

  message("~~~ LDGLITE 3RD INSTALL PREFIX $$shell_path($${3RD_PREFIX}) ~~~")

  target.path                 = $${3RD_BINDIR}
  documentation.path          = $${3RD_DOCDIR}
  documentation.files         = $$_PRO_FILE_PWD_/../doc/ldglite.1 \
                                $$_PRO_FILE_PWD_/../doc/LICENCE \
                                $$_PRO_FILE_PWD_/../doc/README.TXT

  INSTALLS += target documentation

  macx {
    resources.path              = $${3RD_RESOURCES}
    resources.files             = set-ldrawdir.command
    macx: resources.files      += ldglite_w.command

    INSTALLS += resources
  }

} else:unix|msys:!macx {
  # someone asked for the standard linux install routine so here it is...
  isEmpty(PREFIX_):PREFIX_    = $${PREFIX}/usr
  isEmpty(BINDIR):BINDIR      = $${PREFIX_}/bin
  isEmpty(DATADIR):DATADIR    = $${PREFIX_}/share
  isEmpty(DOCDIR):DOCDIR      = $${DATADIR}/doc
  isEmpty(MANDIR):MANDIR      = $${DATADIR}/man

  target.path                 = $${BINDIR}
  documentation.path          = $${DOCDIR}/$${TARGET}
  documentation.files         = $$_PRO_FILE_PWD_/../doc/LICENCE \
                                $$_PRO_FILE_PWD_/../doc/README.TXT
  manual.path                 = $${MANDIR}
  manual.files                = $$_PRO_FILE_PWD_/../doc/ldglite.1

  INSTALLS += target documentation manual
}

# set config to enable build check
# CONFIG+=BUILD_CHECK
# ldglite -l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -2g,2x -w1 -l -ldcFtests/LDConfigCustom01.ldr -mFtests/TestOK_1.3.8_Foo2.png tests/Foo2.ldr
#
# Build Check on Windows:
# Console:
# - Launch console at ldglite repository and execute the following:
# SET LDRAW_DIR=%USERPROFILE%\LDraw
# SET LDRAWDIR=%LDRAW_DIR%
# ECHO -Set LDRAWDIR to %LDRAWDIR%.
# -l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -2g,2x -w1 -l -ldcFtests\LDConfigCustom01.ldr -mFtests\32bit_release-TestOK_1.3.8_Foo2.png tests\Foo2.ldr
# QtCreator:
# Add to Environment:
#   LDRAWDIR=%USERPROFILE%\LDraw
# Add to Run command line arguments: -l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -2g,2x -w1 -l -ldcF..\..\tests\LDConfigCustom01.ldr -mF..\..\tests\32bit_release-TestOK_1.3.8_Foo2.png ..\..\tests\Foo2.ldr
# Copy tests folder to OUTPUT folder - e.g. .../app/32bit_debug/tests
WINDOWS_CHECK = $$(LP3D_WINDOWS_CHECK)
BUILD_CHECK: unix|msys|contains(WINDOWS_CHECK, 1) {
  msys: LDGLITE_EXE = $${TARGET}.exe
  else: LDGLITE_EXE = ./$${TARGET}
  CHECK_DIR   = $${_PRO_FILE_PWD_}/../tests
  RESULT_FILE = $${DESTDIR}-TestOK_$${VERSION}_Foo2.png
  CONFIG_DIR  = $$shell_path($$absolute_path($${CHECK_DIR}/LDConfigCustom01.ldr))
  LDRFILE_DIR = $$shell_path($$absolute_path($${CHECK_DIR}/Foo2.ldr))
  LDRAW_PATH  = $$absolute_path($$(LDRAWDIR))
  exists($${LDRAW_PATH}) {
    message("~~~ LDRAW LIBRARY $${LDRAW_PATH} ~~~")
    equals(PWD, $${OUT_PWD}): RESULT_DIR = $$shell_path(../../tests/$${RESULT_FILE})
    else: RESULT_DIR = $$shell_path($$absolute_path($${CHECK_DIR}/$${RESULT_FILE}))
    QMAKE_POST_LINK += $$escape_expand(\n\t)                                              \
                       cd $${OUT_PWD}/$${DESTDIR} && $${LDGLITE_EXE} -l3 -i2 -ca0.01      \
                       -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -2g,2x -w1 -l \
                       -ldcF$${CONFIG_DIR} -mF$${RESULT_DIR} $${LDRFILE_DIR}
  } else {
    message("WARNING: LDRAW LIBRARY PATH NOT DEFINED - LDGLite CUI cannot be tested")
    QMAKE_POST_LINK += $$escape_expand(\n\t)                                              \
                       cd $${OUT_PWD}/$${DESTDIR} && $${LDGLITE_EXE} -mS -ldcF$${CONFIG_DIR}
  }
}
