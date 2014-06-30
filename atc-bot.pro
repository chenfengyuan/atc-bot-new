TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += main.cpp
INCLUDEPATH += ./include/
QMAKE_CXXFLAGS += -O2 -g -Wall -Wextra -isystem ./include --save-temps
LIBS += -L. -lgtest

HEADERS += \
    atc.hpp \
    atc-utils.hpp \
    atc_search.hpp
