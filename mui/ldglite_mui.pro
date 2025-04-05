TEMPLATE = lib
TARGET   = mui
QT      += core
QT      -= opengl
QT      -= gui
CONFIG 	-= qt
CONFIG  -= opengl
CONFIG  += thread
CONFIG  += staticlib
CONFIG 	+= warn_on

include($$PWD/../ldgliteglobal.pri)

message("~~~ $$upper($$TARGET) $$join(ARCH,,,bit) $$BUILD LIBRARY ~~~")

DEFINES       += TEST_MUI_GUI
macx: DEFINES += USE_GLUT_MENUS
INCLUDEPATH   += $$PWD/src

include($$PWD/ldglitemui.pri)
