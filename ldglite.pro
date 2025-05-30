# qmake Configuration options
# CONFIG+=BUILD_CHECK
# CONFIG+=ENABLE_PNG
# CONFIG+=ENABLE_TILE_RENDERING
# CONFIG+=ENABLE_OFFSCREEN_RENDERING
# CONFIG+=ENABLE_TEST_GUI
# CONFIG+=MAKE_APP_BUNDLE
# CONFIG+=USE_OSMESA_STATIC
# CONFIG+=USE_FREEGLUT_LOCAL # use local freeglut (e.g. Windows, Alma Linux)
# CONFIG+=USE_OSMESA_LOCAL   # use local OSmesa and LLVM libraries - for OBS images w/o OSMesa stuff (e.g. RHEL)
# CONFIG+=OSMESA_NO_LLVM     # LLVM not needed for specified OSMesa configuration
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_linux_3rdparty
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_msys_3rdparty
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_macos_3rdparty
# CONFIG+=3RD_PARTY_INSTALL=../../lpub3d_windows_3rdparty
# LDGLite directory and project file structre
# --------------
# /ldglite.pro
# /ldgliteglobal.pri
#   |
#   |---/app
#   |     |---inherits:ldgliteglobal.pri
#   |     |---ldglite_app.pro
#   |     |---ldgliteapp.pri
#   |     |---tiles.pri
#   |
#   |---/ldrawini
#   |     |---inherits:ldgliteglobal.pri
#   |     |---ldglite_ldrawini.pro
#   |     |---ldgliteldrawini.pri
#   |
#   `---/mui
#         |---inherits:ldgliteglobal.pri
#         |---ldglite_mui.pro
#         |---ldglitemui.pri
#

win32:HOST = $$system(systeminfo | findstr /B /C:\"OS Name\")
unix:!macx:HOST = $$system(. /etc/os-release && if test \"$PRETTY_NAME\" != \"\"; then echo \"$PRETTY_NAME\"; else echo `uname`; fi)
macx:HOST = $$system(echo `sw_vers -productName` `sw_vers -productVersion`)
isEmpty(HOST):HOST = UNKNOWN HOST

!contains(CONFIG, ENABLE_TEST_GUI): CONFIG += ENABLE_TEST_GUI

TEMPLATE=subdirs

# This tells Qt to compile the following SUBDIRS in order
CONFIG  += ordered

SUBDIRS  = ldglite_ldrawini
ldglite_ldrawini.file     = $$PWD/ldrawini/ldglite_ldrawini.pro
ldglite_ldrawini.makefile = Makefile.ldrawini
ldglite_ldrawini.target   = sub-ldglite_ldrawini
ldglite_ldrawini.depends  =

ENABLE_TEST_GUI {
  SUBDIRS += ldglite_mui
  ldglite_mui.file        = $$PWD/mui/ldglite_mui.pro
  ldglite_mui.makefile    = Makefile.mui
  ldglite_mui.target      = sub-ldglite_mui
  ldglite_mui.depends     =
}

SUBDIRS += ldglite_app
ldglite_app.file          = $$PWD/app/ldglite_app.pro
ldglite_app.makefile      = Makefile.app
ldglite_app.target        = sub-ldglite_app
ldglite_app.depends       = ldglite_ldrawini

ENABLE_TEST_GUI {
  ldglite_app.depends     = ldglite_mui
}

OTHER_FILES += \
    $$PWD/doc/ldglite.1 \
    $$PWD/doc/README.TXT \
    $$PWD/set-ldrawdir.command \
    $$PWD/.travis.yml \
    $$PWD/appveyor.yml \
    $$PWD/build.cmd \
    $$PWD/.github/workflows/build.yml \
    $$PWD/utils/install-dev-packages.sh \
    $$PWD/utils/ldglite_osxwrapper.sh \
    $$PWD/utils/ledit \
    $$PWD/tests/LDConfigCustom01.ldr \
    $$PWD/tests/testOSX:sh \
    $$PWD/tests/testWin.bat \
    $$PWD/obs/debian/control \
    $$PWD/obs/debian/rules \
    $$PWD/obs/ldglite.spec \
    $$PWD/obs/PKGBUILD

BUILD_ARCH = $$(TARGET_CPU)
isEmpty(BUILD_ARCH): \
!contains(QT_ARCH, unknown): \
BUILD_ARCH = $$QT_ARCH
isEmpty(BUILD_ARCH): BUILD_ARCH = UNKNOWN ARCH
CONFIG(debug, debug|release): BUILD = DEBUG BUILD
else:                         BUILD = RELEASE BUILD
msys:                         BUILD = MSYS $${BUILD}
message("~~~ LDGLITE $$upper($$BUILD_ARCH) $${BUILD} ON $$upper($$HOST) ~~~")
msys: message("~~~ MSYS2 SYSTEM_PREFIX $${PREFIX} ~~~ ")
