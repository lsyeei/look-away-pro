#include "SystemTrayIcon.h"
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>
#include <QTimer>
#include <QDebug>

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
    , m_menu(nullptr)
{
    // Load icon from resource
    QIcon icon(":/icon.ico");

    if (icon.isNull()) {
        // Fallback: create icon programmatically if resource file not available
        QPixmap pixmap(64, 64);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        // Eye outer shape
        painter.setPen(QPen(QColor(60, 120, 180), 4));
        painter.setBrush(QBrush(QColor(200, 220, 255)));
        painter.drawEllipse(4, 18, 56, 30);

        // Iris
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(80, 150, 220)));
        painter.drawEllipse(18, 24, 28, 18);

        // Pupil
        painter.setBrush(QBrush(QColor(20, 40, 80)));
        painter.drawEllipse(26, 28, 12, 10);

        // Reflection
        painter.setBrush(QBrush(QColor(255, 255, 255, 180)));
        painter.drawEllipse(32, 24, 4, 4);

        painter.end();

        icon = QIcon(pixmap);
    }

    setIcon(icon);
    setToolTip("Look Away Pro - 护眼提醒工具");

    createActions();
    createMenu();
    
    // Connect activated signal to handle left click
    connect(this, &QSystemTrayIcon::activated, this, &SystemTrayIcon::onActivated);
    
    // Pre-load menu by showing it off-screen (this makes first real popup fast)
    if (m_menu) {
        // Show at an off-screen position briefly, then hide
        m_menu->popup(QPoint(-1000, -1000));
        QTimer::singleShot(10, [this]() {
            if (m_menu) {
                m_menu->hide();
                qDebug() << "Menu pre-loaded successfully";
            }
        });
    }
}

void SystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        // Left click shows the menu at cursor position
        if (m_menu) {
            m_menu->popup(QCursor::pos());
        }
    }
}

void SystemTrayIcon::setPaused(bool paused)
{
    if (m_pauseAction) {
        m_pauseAction->setText(paused ? "继续" : "暂停");
        m_pauseAction->setIcon(paused ? QIcon(":/icons/play.svg") : QIcon(":/icons/pause.svg"));
    }
}

void SystemTrayIcon::createActions()
{
    // Pre-load SVG icons to cache for faster first-time display
    QIcon statsIcon(":/icons/statistics.svg");
    QIcon settingsIcon(":/icons/settings.svg");
    QIcon pauseIcon(":/icons/pause.svg");
    QIcon aboutIcon(":/icons/about.svg");
    QIcon quitIcon(":/icons/quit.svg");
    QIcon timerIcon(":/icons/timer.svg");
    QIcon countdownIcon(":/icons/countdown.svg");
    
    // Force icons to be rendered (pre-cache)
    statsIcon.pixmap(16, 16);
    settingsIcon.pixmap(16, 16);
    pauseIcon.pixmap(16, 16);
    aboutIcon.pixmap(16, 16);
    quitIcon.pixmap(16, 16);
    timerIcon.pixmap(16, 16);
    countdownIcon.pixmap(16, 16);
    
    m_statisticsAction = new QAction(statsIcon, "统计", this);
    connect(m_statisticsAction, &QAction::triggered, this, &SystemTrayIcon::showStatistics);

    m_settingsAction = new QAction(settingsIcon, "选项", this);
    connect(m_settingsAction, &QAction::triggered, this, &SystemTrayIcon::showSettings);

    m_pauseAction = new QAction(pauseIcon, "暂停", this);
    connect(m_pauseAction, &QAction::triggered, this, &SystemTrayIcon::pauseResume);

    m_aboutAction = new QAction(aboutIcon, "关于", this);
    connect(m_aboutAction, &QAction::triggered, this, &SystemTrayIcon::showAbout);

    m_quitAction = new QAction(quitIcon, "退出", this);
    connect(m_quitAction, &QAction::triggered, this, &SystemTrayIcon::quit);
    
    m_timerAction = new QAction(timerIcon, "计时器", this);
    connect(m_timerAction, &QAction::triggered, this, &SystemTrayIcon::showTimer);
    
    m_countdownAction = new QAction(countdownIcon, "倒计时", this);
    connect(m_countdownAction, &QAction::triggered, this, &SystemTrayIcon::showCountdown);
}

void SystemTrayIcon::createMenu()
{
    // Create menu with parent (this) for proper memory management
    m_menu = new QMenu(nullptr);
    
    // Use absolute minimal stylesheet - complex QSS causes first-time slowdown
    // Let the system use default styling for best performance
    m_menu->setStyleSheet(
        ".QMenu{padding: 10px 10px;}"
        ".QMenu::item {padding: 6px 18px 6px 18px;}"
        ".QMenu::icon{margin-left:18px;}"
        ".QMenu::item:selected {"
        " background-color: palette(highlight);"
        "color: palette(highlighted-text);"
        "}"
    );

    QIcon toolsIcon(":/icons/tools.svg");
    toolsIcon.pixmap(16, 16);
    // Create tools submenu with icon
    m_toolsMenu = new QMenu();
    m_toolsMenu->setTitle("工具");
    m_toolsMenu->setIcon(toolsIcon);
    m_toolsMenu->addAction(m_timerAction);
    m_toolsMenu->addAction(m_countdownAction);
    
    // Add actions
    m_menu->addAction(m_statisticsAction);
    m_menu->addSeparator();
    m_menu->addAction(m_settingsAction);
    m_menu->addSeparator();
    m_menu->addAction(m_pauseAction);
    m_menu->addSeparator();
    m_menu->addMenu(m_toolsMenu);
    m_menu->addSeparator();
    m_menu->addAction(m_aboutAction);
    m_menu->addSeparator();
    m_menu->addAction(m_quitAction);

    setContextMenu(m_menu);
    
    // Ensure menu is properly set
    if (contextMenu() == m_menu) {
        qDebug() << "System tray menu set successfully";
    } else {
        qWarning() << "Failed to set system tray menu";
    }
}
