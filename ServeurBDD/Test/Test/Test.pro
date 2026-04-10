QT += core network websockets
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# Identification BTS CIEL - Étudiant 3 [cite: 638]
TARGET = TestConnectivite
TEMPLATE = app

SOURCES += main.cpp \
           server.cpp

HEADERS += server.h
