#-------------------------------------------------
#
# Project created by QtCreator 2012-05-20T13:12:29
#
#-------------------------------------------------

QT       += core gui sql multimedia

TARGET = Pomodorush
TEMPLATE = app

include (src/sql/sql.pri)

INCLUDEPATH *= src

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/activitydialog.cpp \
    src/timerdialog.cpp \
    src/widgets/pHeaderView.cpp \
    src/estimationdelegate.cpp \
    src/tododelegate.cpp \
    src/tododialog.cpp \
    src/interruptiondialog.cpp \
    src/reestimationdialog.cpp

HEADERS  += src/mainwindow.h \
    src/activitydialog.h \
    src/timerdialog.h \
    src/widgets/pHeaderView.h \
    src/estimationdelegate.h \
    src/tododelegate.h \
    src/tododialog.h \
    src/interruptiondialog.h \
    src/reestimationdialog.h

FORMS    += src/mainwindow.ui \
    src/activitydialog.ui \
    src/timerdialog.ui \
    src/tododialog.ui \
    src/interruptiondialog.ui \
    src/reestimationdialog.ui

RESOURCES += \
    resources.qrc
