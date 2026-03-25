QT += core sql widgets
QT -= gui

TARGET = InterfacePcClient_Test
CONFIG += console c++11
TEMPLATE = app

INCLUDEPATH += /usr/lib/python3/dist-packages/mypyc/external/googletest/include \
               /usr/lib/python3/dist-packages/mypyc/external/googletest

# On garde uniquement TES fichiers sources ici
SOURCES += main.cpp \
           test_accessbdd.cpp \
           accessbdd.cpp

HEADERS += accessbdd.h

LIBS += -lpthread
