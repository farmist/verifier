TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS+= -std=c++11
SOURCES += main.cpp \
    extractdata.cpp \
    sqlite/sqlite3.c

HEADERS += \
    extractdata.h \
    sqlite/sqlite3.h
