QT += multimedia gui core widgets av avwidgets

unix:QMAKE_CXXFLAGS += -std=c++11
win32:CONFIG += windows c++11

DESTDIR = bin

TARGET = pDub

OBJECTS_DIR = objects

MOC_DIR = objects

INCLUDEPATH += include

LIBS += -lavcodec -lavformat -lavutil -lswresample

HEADERS = include/common.hpp \
          include/pdub.hpp \
          include/microphone.hpp \
          include/record.hpp \
          include/windows/control_window.hpp \
          include/windows/video_window.hpp \
          include/windows/editor_window.hpp \
          include/ffmpeg/error_codes.hpp \
          include/ffmpeg/exception.hpp \
          include/ffmpeg/ffmpeg.hpp \
          include/dialogs/newproject_dialog.hpp \
          include/slider_editor.hpp \
          include/record_workplace.hpp \



SOURCES = src/main.cpp \
          src/pdub.cpp \
          src/control_window.cpp \
          src/video_window.cpp \
          src/editor_window.cpp \
          src/microphone.cpp \
          src/record.cpp \
          src/common.cpp \
          src/ffmpeg/exception.cpp \
          src/ffmpeg/ffmpeg.cpp \
          src/newproject_dialog.cpp \
          src/slider_editor.cpp \
          src/record_workplace.cpp \
