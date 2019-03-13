#-------------------------------------------------
#
# Project created by QtCreator 2019-03-05T10:54:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mainwindow
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    gpsdecode/UI/satellitewidget.cpp \
    gpsdecode/gpsdecode.cpp \
    gpsdecode/GPSFix.cpp \
    gpsdecode/GPSService.cpp \
    gpsdecode/NMEACommand.cpp \
    gpsdecode/NMEAParser.cpp \
    gpsdecode/NumberConversion.cpp \
    gpsdecode/ubx.c \
    mx6s_v4l2_capture_uyvy.c \
    dialog.cpp

HEADERS += \
    gpsdecode/UI/satellitewidget.h \
    gpsdecode/Event.h \
    gpsdecode/gpsdecode.h \
    gpsdecode/GPSFix.h \
    gpsdecode/GPSService.h \
    gpsdecode/nmea.h \
    gpsdecode/NMEACommand.h \
    gpsdecode/NMEAParser.h \
    gpsdecode/NumberConversion.h \
    gpsdecode/ubx.h \
    mx6s_v4l2_capture_uyvy.h \
    mainwindow.h \
    dialog.h \
    mainwindow.h \
    ui_mainwindow.h \
    ../build-MainWindow-Desktop_Qt_5_12_0_MinGW_64_bit-Debug/ui_dialog.h \
    ../build-MainWindow-Desktop_Qt_5_12_0_MinGW_64_bit-Debug/ui_mainwindow.h

FORMS += \
        mainwindow.ui \
        dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
