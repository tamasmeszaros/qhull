# -------------------------------------------------
# rboxr.pro -- Qt project for rboxr.exe
# -------------------------------------------------

TARGET = rboxr

include(../qhull-warn.pri)

DESTDIR = ../../bin
TEMPLATE = app
CONFIG += console warn_on
CONFIG -= qt

LIBS += -L../../lib
build_pass:CONFIG(debug, debug|release){
   LIBS += -lqhullstatic_rd
   OBJECTS_DIR = Debug
}else:build_pass:CONFIG(release, debug|release){
   LIBS += -lqhullstatic_r
   OBJECTS_DIR = Release
}
win32-msvc* : QMAKE_LFLAGS += /INCREMENTAL:NO

INCLUDEPATH += ../libqhullr
CONFIG += qhull_warn_conversion


SOURCES += rbox_r.c