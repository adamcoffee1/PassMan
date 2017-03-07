#-------------------------------------------------
#
# Project created by QtCreator 2017-02-15T14:35:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PassMan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=\
        passman.cpp \
    database.cpp \
    entry.cpp \
    yubikeytester.cpp \
    yubikey.cpp \
    authenticator.cpp \
    main.cpp \
    about.cpp \
    generator.cpp \
    strengthcalculator.cpp \
    help.cpp \
    license.cpp

HEADERS  += passman.h \
    database.h \
    entry.h \
    yubikeytester.h \
    yubikey.h \
    authenticator.h \
    about.h \
    generator.h \
    strengthcalculator.h \
    help.h \
    license.h

FORMS    += passman.ui \
    yubikeytester.ui \
    authenticator.ui \
    about.ui \
    generator.ui \
    strengthcalculator.ui \
    help.ui \
    license.ui

DISTFILES +=

RESOURCES += \
    passmanresources.qrc

LIBS += -L/usr/lib/libcrypto++.a -lcrypto++
