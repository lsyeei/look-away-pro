#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include "SystemTrayIcon.h"
#include "TimerManager.h"
#include "ReminderDialog.h"
#include "StatisticsDialog.h"
#include "SettingsDialog.h"
#include "DatabaseManager.h"
#include "screenstatemonitor.h"

class AudioPlayer;

class ConfigManager;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onShowStatistics();
    void onShowSettings();
    void onPauseResume();
    void onShowAbout();
    void onQuit();
    void onLookAwayTrigger();
    void onWorkStart();
    void onWorkEnd();
    void onScreenStateChanged(ScreenState state);
    void onShowTimer();
    void onShowCountdown();

private:
    void setupTrayIcon();
    void loadSettings();
    void setupAutoStart(bool enabled);
    QString getAppPath() const;

    SystemTrayIcon *m_trayIcon;
    TimerManager *m_timerManager;
    ReminderDialog *m_reminderDialog;
    StatisticsDialog *m_statisticsDialog;
    SettingsDialog *m_settingsDialog;
    DatabaseManager *m_databaseManager;
    ScreenStateMonitor *m_screenMonitor;
    ConfigManager* m_config;
    class TimerWindow *m_timerWindow;
    class CountdownWindow *m_countdownWindow;

    bool m_isPaused;
    bool m_screenSaverActive;
};

#endif // MAINWINDOW_H
