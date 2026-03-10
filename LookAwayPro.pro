QT       += core gui widgets sql multimediawidgets xml concurrent

TARGET = LookAwayPro
TEMPLATE = app

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    SystemTrayIcon.cpp \
    TimerManager.cpp \
    ReminderDialog.cpp \
    StatisticsDialog.cpp \
    SettingsDialog.cpp \
    DatabaseManager.cpp \
    AboutDialog.cpp \
    screenstatemonitor.cpp \
    ConfigManager.cpp \
    TimerWindow.cpp \
    CountdownWindow.cpp \
    CountdownInputDialog.cpp \
    AudioPlayer.cpp

HEADERS += \
    MainWindow.h \
    SystemTrayIcon.h \
    TimerManager.h \
    ReminderDialog.h \
    StatisticsDialog.h \
    SettingsDialog.h \
    DatabaseManager.h \
    AboutDialog.h \
    screenstatemonitor.h \
    ConfigManager.h \
    TimerWindow.h \
    CountdownWindow.h \
    CountdownInputDialog.h \
    AudioPlayer.h

FORMS += \
    StatisticsDialog.ui \
    SettingsDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_ICONS = icon.ico

win32 {
    RC_FILE = LookAwayPro.rc
    LIBS += -luser32 -lwtsapi32 -lole32
    DEFINES += _WIN32_WINNT=0x0601
}

linux {
    QT += dbus
}

macx {
    CONFIG += objective_c
    LIBS += -framework Cocoa -framework CoreGraphics
    # For macOS, we need to explicitly specify Objective-C++ for some files
    OBJECTIVE_SOURCES += screenstatemonitor.cpp
}
