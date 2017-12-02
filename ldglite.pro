# LDGLite directory and project file structre
# --------------
# /ldglite.pro
#   |
#   |---ldglite_app.pro
#   |---ldgliteglobal.pri
#   |
#   `---/ldrawini
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

CONFIG(debug, debug|release) {
    message("~~~ LDGLITE DEBUG BUILD ON $$upper($$HOST) ~~~")
} else {
    message("~~~ LDGLITE RELEASE BUILD ON $$upper($$HOST) ~~~")
}

OTHER_FILES += \
    $$PWD/doc/ldglite.1 \
    $$PWD/doc/README.TXT \
    $$PWD/set-ldrawdir.command \
    $$PWD/.travis.yml \
    $$PWD/appveyor.yml
