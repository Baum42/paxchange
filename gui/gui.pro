TEMPLATE = app

QT       += core gui widgets

TARGET = pacsync
VERSION = $$PACSYNCVER

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
		mainwindow.h

SOURCES += \
		main.cpp \
		mainwindow.cpp

FORMS += \
		mainwindow.ui

unix {
	target.path = $$[QT_INSTALL_BINS]
	INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lpacsync
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lpacsync
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lpacsync

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib
