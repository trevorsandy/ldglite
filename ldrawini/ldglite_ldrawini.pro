TEMPLATE = lib
TARGET   = ldrawini
QT      += core
QT      -= opengl
QT      -= gui
CONFIG 	-= qt
CONFIG  -= opengl
CONFIG  += thread
CONFIG  += staticlib
CONFIG 	+= warn_on

include($$PWD/../ldgliteglobal.pri)

win32: VERSION = 16.1.8.0  # major.minor.patch.build
else: VERSION = 16.1.8     # major.minor.patch

message("~~~ $$upper($$TARGET) $$join(ARCH,,,bit) $$BUILD LIBRARY VERSION $$VERSION ~~~")

win32 {

    QMAKE_EXT_OBJ = .obj

    QMAKE_TARGET_COMPANY = "Lars C. Hassing"
    QMAKE_TARGET_DESCRIPTION = "LDrawDir and SearchDirs API"
    QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2004-2008  Lars C. Hassing"
    QMAKE_TARGET_PRODUCT = "LDrawIni ($$join(ARCH,,,bit))"

}

include($$PWD/ldgliteldrawini.pri)
