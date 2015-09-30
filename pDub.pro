QT += widgets

unix:QMAKE_CXXFLAGS += -std=c++11
win32:CONFIG += windows c++11

DESTDIR = bin

TARGET = pDub

OBJECTS_DIR = objects

MOC_DIR = objects

INCLUDEPATH += include

HEADERS = include/common.hpp \
          include/pdub.hpp \
          include/windows/control_window.hpp \
          include/windows/video_window.hpp \
          include/windows/editor_window.hpp


SOURCES = src/main.cpp \
          src/pdub.cpp \
          src/control_window.cpp \
          src/video_window.cpp \
          src/editor_window.cpp
