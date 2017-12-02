TEMPLATE = lib
TARGET   = mui

include($$PWD/../ldgliteglobal.pri)

message("~~~ MUI $$join(ARCH,,,bit) $$BUILD LIBRARY ~~~")

DEFINES       += TEST_MUI_GUI
macx: DEFINES += USE_GLUT_MENUS
INCLUDEPATH   += $$PWD/src

include($$PWD/ldglitemui.pri)
