TEMPLATE = lib
TARGET   = ldrawini

include($$PWD/../ldgliteglobal.pri)

win32: VERSION = 16.1.8.0  # major.minor.patch.build
else: VERSION = 16.1.8     # major.minor.patch

message("~~~ LDRAWINI $$join(ARCH,,,bit) $$BUILD LIBRARY VERSION $$VERSION ~~~")

win32 {

    QMAKE_EXT_OBJ = .obj
    CONFIG += windows

    QMAKE_TARGET_COMPANY = "Lars C. Hassing"
    QMAKE_TARGET_DESCRIPTION = "LDrawDir and SearchDirs API"
    QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2004-2008  Lars C. Hassing"
    QMAKE_TARGET_PRODUCT = "LDrawIni ($$join(ARCH,,,bit))"

}

include($$PWD/ldgliteldrawini.pri)
