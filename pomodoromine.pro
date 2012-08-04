#-------------------------------------------------
#
# Project created by QtCreator 2012-03-11T16:02:17
#
#-------------------------------------------------

QT       += phonon core gui network script xmlpatterns

unix {
    QT += dbus
}

TARGET = pomodoromine
TEMPLATE = app

SOURCES +=  src/main.cpp\
            src/mainwindow.cpp \
            src/taskquery.cpp \
            src/settingsdialog.cpp \
            src/newtaskdialog.cpp

HEADERS  += src/mainwindow.h \
            src/taskquery.h \
            src/settingsdialog.h \
            src/newtaskdialog.h

FORMS    += ui/mainwindow.ui \
            ui/settingsdialog.ui \
            ui/newtaskdialog.ui

RESOURCES += \
    pomodoromine.qrc


# The application version
VERSION = 0.1

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
