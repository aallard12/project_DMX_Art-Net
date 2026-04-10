QT += core network sql
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = serveurtcp
TEMPLATE = app

SOURCES += main.cpp serveurtcp.cpp
HEADERS += serveurtcp.h
