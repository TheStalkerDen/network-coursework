QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    global.cpp \
    main.cpp \
    mainscene.cpp \
    mainview.cpp \
    mainwindow.cpp \
    networkline.cpp \
    networknode.cpp \
    networkpackage.cpp \
    networkpath.cpp \
    routingtable.cpp \
    routingtablewidget.cpp \
    simulationdatawidget.cpp

HEADERS += \
    global.h \
    mainscene.h \
    mainview.h \
    mainwindow.h \
    networkline.h \
    networknode.h \
    networkpackage.h \
    networkpath.h \
    routingtable.h \
    routingtablewidget.h \
    simulationdatawidget.h

FORMS += \
    mainwindow.ui \
    routingtablewidget.ui \
    simulationdatawidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


QMAKE_EXTRA_TARGETS += before_build makefilehook

makefilehook.target = $(MAKEFILE)
makefilehook.depends = .beforebuild

PRE_TARGETDEPS += .beforebuild

before_build.target = .beforebuild
before_build.depends = FORCE
before_build.commands = chcp 1251
