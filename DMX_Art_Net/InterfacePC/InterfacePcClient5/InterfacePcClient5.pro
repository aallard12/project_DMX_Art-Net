QT       += core gui sql network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    accessbdd.cpp \
    dialogunivers.cpp \
    main.cpp \
    interfacepcclient.cpp

HEADERS += \
    accessbdd.h \
    dialogunivers.h \
    interfacepcclient.h

FORMS += \
    dialogunivers.ui \
    interfacepcclient.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS += -fno-stack-protector
QMAKE_CFLAGS   += -fno-stack-protector
