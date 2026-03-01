#include "MainWindow.h"
#include "AboutDialog.h"
#include "screenstatemonitor.h"
#include "ConfigManager.h"
#include "TimerWindow.h"
#include "CountdownWindow.h"
#include "CountdownInputDialog.h"
#include <QApplication>
#include <QScreen>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QIcon>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QColor>
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_trayIcon(nullptr)
    , m_timerManager(nullptr)
    , m_reminderDialog(nullptr)
    , m_statisticsDialog(nullptr)
    , m_settingsDialog(nullptr)
    , m_databaseManager(nullptr)
    , m_screenMonitor(nullptr)
    , m_config(nullptr)
    , m_timerWindow(nullptr)
    , m_countdownWindow(nullptr)
    , m_isPaused(false)
    , m_screenSaverActive(false)
{
    setWindowTitle("Look Away Pro");
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setWindowIcon(QIcon(":/icon.ico"));
    resize(1, 1);

    // Load settings using ConfigManager
    m_config = ConfigManager::instance();
    if (!m_config) {
        qCritical("Failed to create ConfigManager instance");
        return;
    }
    // load configuration
    m_config->loadConfig();

    // Initialize database manager
    m_databaseManager = new DatabaseManager(this);
    if (!m_databaseManager) {
        qCritical("Failed to create DatabaseManager");
        return;
    }

    // Check if system tray is available before setting it up
    bool trayAvailable = QSystemTrayIcon::isSystemTrayAvailable();
    if (!trayAvailable) {
        qWarning("系统托盘不可用，程序将以窗口模式运行");
    } else {
        // Setup system tray icon (only if tray is available)
        setupTrayIcon();
    }

    // Initialize timer manager
    m_timerManager = new TimerManager(this);
    if (!m_timerManager) {
        qCritical("Failed to create TimerManager");
        return;
    }
    connect(m_timerManager, &TimerManager::timerComplete, this, &MainWindow::onTimerComplete);
    connect(m_timerManager, &TimerManager::sessionStart, this, &MainWindow::onSessionStart);
    connect(m_timerManager, &TimerManager::sessionEnd, this, &MainWindow::onSessionEnd);

    loadSettings();

    // Initialize screen monitor (as member variable, not local)
    m_screenMonitor = new ScreenStateMonitor(this);
    if (!m_screenMonitor) {
        qCritical("Failed to create ScreenStateMonitor");
        return;
    }
    connect(m_screenMonitor, &ScreenStateMonitor::screenStateChanged, this, &MainWindow::onScreenStateChanged);
    
    // Start timer after everything is initialized
    m_timerManager->start();
    
    // Minimize to system tray after starting timer (only if tray is available)
    if (trayAvailable) {
        hide();
    } else {
        // If no system tray, keep window visible but minimized
        showMinimized();
        qDebug() << "程序正在运行（系统托盘不可用）";
    }
}

MainWindow::~MainWindow()
{
    // Qt will automatically delete child objects, but we should stop timers first
    if (m_timerManager) {
        m_timerManager->stop();
    }
    
    // Database manager should be explicitly closed
    if (m_databaseManager) {
        m_databaseManager->close();
    }
}

void MainWindow::setupTrayIcon()
{
    m_trayIcon = new SystemTrayIcon(this);
    if (!m_trayIcon) {
        qCritical("Failed to create SystemTrayIcon");
        return;
    }

    connect(m_trayIcon, &SystemTrayIcon::showStatistics, this, &MainWindow::onShowStatistics);
    connect(m_trayIcon, &SystemTrayIcon::showSettings, this, &MainWindow::onShowSettings);
    connect(m_trayIcon, &SystemTrayIcon::pauseResume, this, &MainWindow::onPauseResume);
    connect(m_trayIcon, &SystemTrayIcon::showAbout, this, &MainWindow::onShowAbout);
    connect(m_trayIcon, &SystemTrayIcon::quit, this, &MainWindow::onQuit);
    connect(m_trayIcon, &SystemTrayIcon::showTimer, this, &MainWindow::onShowTimer);
    connect(m_trayIcon, &SystemTrayIcon::showCountdown, this, &MainWindow::onShowCountdown);

    // Show the tray icon
    m_trayIcon->show();
    
    // Verify it's actually visible
    if (!m_trayIcon->isVisible()) {
        qWarning() << "System tray icon is not visible after show()";
    } else {
        qDebug() << "System tray icon is visible";
    }
    
    // On Windows, sometimes we need to process events to make the icon appear
    QApplication::processEvents();
}

void MainWindow::loadSettings()
{
    m_timerManager->setWorkTime(m_config->workTime());
    m_timerManager->setBreakTime(m_config->breakTime());

    setupAutoStart(m_config->autoStart());
}

void MainWindow::setupAutoStart(bool enabled)
{
    QString appPath = QApplication::applicationFilePath();

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      QSettings::NativeFormat);

    if (enabled) {
        settings.setValue("LookAwayPro", appPath.replace('/', '\\'));
    } else {
        settings.remove("LookAwayPro");
    }
}

void MainWindow::onShowStatistics()
{
    if (!m_statisticsDialog) {
        m_statisticsDialog = new StatisticsDialog(m_databaseManager, this);
    }

    m_statisticsDialog->show();
    m_statisticsDialog->activateWindow();
}

void MainWindow::onShowSettings()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
        connect(m_settingsDialog, &SettingsDialog::settingsChanged, this, [this]() {
            loadSettings();
            // Restart the timer with new settings
            m_timerManager->reset();
            m_timerManager->start();
        });
    }
    m_settingsDialog->loadSettings();
    m_settingsDialog->show();
    m_settingsDialog->activateWindow();
}

void MainWindow::onPauseResume()
{
    if (m_isPaused) {
        m_timerManager->resume();
        m_trayIcon->setPaused(false);
    } else {
        m_timerManager->pause();
        m_trayIcon->setPaused(true);
    }
    m_isPaused = !m_isPaused;
}

void MainWindow::onShowAbout()
{
    AboutDialog aboutDialog(this);
    
    // Center the dialog on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - aboutDialog.width()) / 2;
    int y = (screenGeometry.height() - aboutDialog.height()) / 2;
    aboutDialog.move(x, y);
    
    aboutDialog.exec();
}

void MainWindow::onQuit()
{
    m_databaseManager->close();
    qApp->quit();
}

void MainWindow::onShowTimer()
{
    if (!m_timerWindow) {
        m_timerWindow = new TimerWindow(nullptr);  // Use nullptr instead of this
        connect(m_timerWindow, &QWidget::destroyed, [this]() {
            m_timerWindow = nullptr;
        });
        qDebug() << "TimerWindow created";
    }
    
    // Ensure window is visible and not minimized
    m_timerWindow->setWindowState(Qt::WindowActive);
    m_timerWindow->showNormal();
    m_timerWindow->show();
    m_timerWindow->raise();
    m_timerWindow->activateWindow();
    
    qDebug() << "TimerWindow shown. Visible:" << m_timerWindow->isVisible() 
             << "Geometry:" << m_timerWindow->geometry();
}

void MainWindow::onShowCountdown()
{
    class CountdownInputDialog *inputDialog = new CountdownInputDialog(this);
    if (inputDialog->exec() == QDialog::Accepted) {
        int hours = inputDialog->hours();
        int minutes = inputDialog->minutes();
        int seconds = inputDialog->seconds();
        
        if (hours > 0 || minutes > 0 || seconds > 0) {
            if (m_countdownWindow) {
                m_countdownWindow->close();
                m_countdownWindow->deleteLater();
            }
            
            m_countdownWindow = new CountdownWindow(hours, minutes, seconds, nullptr);  // Use nullptr instead of this
            
            // Ensure window is visible and not minimized
            m_countdownWindow->setWindowState(Qt::WindowActive);
            m_countdownWindow->showNormal();
            m_countdownWindow->show();
            m_countdownWindow->raise();
            m_countdownWindow->activateWindow();
            
            qDebug() << "CountdownWindow shown. Visible:" << m_countdownWindow->isVisible()
                     << "Geometry:" << m_countdownWindow->geometry();
            
            connect(m_countdownWindow, &QWidget::destroyed, [this]() {
                m_countdownWindow = nullptr;
            });
        }
    }
    
    inputDialog->deleteLater();
}

void MainWindow::onTimerComplete()
{

    if (!m_reminderDialog) {
        m_reminderDialog = new ReminderDialog(this);
    }

    m_reminderDialog->showFullScreen();
    m_reminderDialog->exec();

    // Record break time after dialog closes
    m_databaseManager->recordBreakTime(m_config->breakTime());
    // start next timer
    m_timerManager->start();
}

void MainWindow::onSessionStart()
{
    m_databaseManager->startWorkSession();
}

void MainWindow::onSessionEnd()
{
    m_databaseManager->endWorkSession();
}

void MainWindow::onScreenStateChanged(ScreenState state)
{
    // option 1-stop  2-restart
    switch (state) {
    case ScreenState::Unlocked:
        // 从普通状态解锁（非锁屏状态）
        if(/*m_config->screenSaverOption() == 1 && */!m_timerManager->isRunning()){
        //     // resume timer
        //     m_timerManager->resume();qDebug() << "Screen unlocked, resume timer";
        // }else if(m_config->screenSaverOption() == 2 && !m_timerManager->isRunning()){
        //     // restart timer
            bool loggedIn{false};
            // 检查从锁屏解锁后用户状态
            if (m_screenMonitor) {
                loggedIn = m_screenMonitor->isUserLoggedIn();
                qDebug() << "System unlocked - User logged in:" << loggedIn;
            }
            if(loggedIn){
                m_timerManager->start();qDebug() << "从普通状态解锁（非锁屏状态）, restart timer";
            }
        }
        break;
        
    case ScreenState::UnlockedFromLock:
        // 从系统锁屏解锁
        if(m_config->lockScreenOption() == 1 && !m_timerManager->isRunning()){
            // resume timer
            m_timerManager->resume();qDebug() << "Screen unlocked from lock, resume timer";
        }else if(m_config->lockScreenOption() == 2 && !m_timerManager->isRunning()){
            // restart timer
            m_timerManager->start();qDebug() << "Screen unlocked from lock, restart timer";
        }
        break;
        
    case ScreenState::ScreenSaverStart:
        // 屏保启动
        if(m_config->screenSaverOption() == 1 && m_timerManager->isRunning()){
            // pause timer
            m_timerManager->pause();qDebug() << "Screensaver started, pause timer";
        }else if(m_config->screenSaverOption() == 2 && m_timerManager->isRunning()){
            // stop timer
            m_timerManager->stop();qDebug() << "Screensaver started, stop timer";
        }
        break;
        
    case ScreenState::ScreenSaverStop:
        // 屏保退出,检查屏保退出后用户是否已经登录
        if (m_screenMonitor) {
            bool loggedIn = m_screenMonitor->isUserLoggedIn();
            qDebug() << "Screensaver stopped - User logged in:" << loggedIn;
            if (loggedIn) {
                qDebug() << "User needs to enter password to unlock";
                // 密码解锁界面的处理逻辑
                if(m_config->screenSaverOption() == 1 && !m_timerManager->isRunning()){
                    // resume timer
                    m_timerManager->resume();qDebug() << "Screensaver stopped, resume timer";
                }else if(m_config->screenSaverOption() == 2 && !m_timerManager->isRunning()){
                    // restart timer
                    m_timerManager->start();qDebug() << "Screensaver stopped, restart timer";
                }
            }
        }
        break;
        
    case ScreenState::Locked:
        // 系统锁屏
        if(m_config->lockScreenOption() == 1 && m_timerManager->isRunning()){
            // pause timer
            m_timerManager->pause();qDebug() << "System locked, pause timer";
        }else if(m_config->lockScreenOption() == 2 && m_timerManager->isRunning()){
            // stop timer
            m_timerManager->stop();qDebug() << "System locked, stop timer";
        }
        break;
    }
}

QString MainWindow::getAppPath() const
{
    return QApplication::applicationFilePath();
}
