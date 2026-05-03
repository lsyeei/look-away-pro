#include "TimerWindow.h"
#include "ConfigManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>
#include <QApplication>
#include <QScreen>

TimerWindow::TimerWindow(QWidget *parent)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_time(0, 0, 0)
    , m_timeLabel(new QLabel(this))
    , m_playPauseButton(new QPushButton(this))
    , m_restartButton(new QPushButton(this))
    , m_closeButton(new QPushButton(this))
    , m_running(false)
    , m_started(false), m_saverRunning(false)
{
    setupUI();

    connect(m_timer, &QTimer::timeout, this, &TimerWindow::onTimerTimeout);
    connect(m_playPauseButton, &QPushButton::clicked, this, &TimerWindow::onPlayPauseClicked);
    connect(m_restartButton, &QPushButton::clicked, this, &TimerWindow::onStopClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &TimerWindow::onCloseClicked);
    connect(ConfigManager::instance(), &ConfigManager::configChanged,
            this, [&](){updateTimeStyle();});
    m_timer->setInterval(1000);
    m_timer->start();
    m_timeLabel->setText(m_time.toString("HH:mm:ss"));
}

TimerWindow::~TimerWindow()
{
}

void TimerWindow::updateTimeStyle()
{
    auto config = ConfigManager::instance();
    QString fontFamily = config->timerFontFamily();
    int fontSize = config->timerFontSize();
    QColor textColor = config->timerTextColor();

    if (fontFamily.isEmpty() || fontSize <= 0) {
        fontFamily = "Arial";
        fontSize = 24;
        textColor = Qt::white;
    }

    QFont font(fontFamily, fontSize);
    font.setBold(true);
    m_timeLabel->setFont(font);
    m_timeLabel->setStyleSheet(QString("color: %1;").arg(textColor.name()));
}

void TimerWindow::showButton(bool flag)
{
    m_playPauseButton->setVisible(flag);
    m_restartButton->setVisible(flag);
    m_closeButton->setVisible(flag);
}

void TimerWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_Hover, true);
    // 不接受焦点
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_ShowWithoutActivating);
    
    // Ensure minimum size
    setMinimumSize(200, 100);
    
    updateTimeStyle();
    m_timeLabel->setAlignment(Qt::AlignCenter);
    
    updateDisplay();
    
    m_playPauseButton->setFixedSize(30, 30);
    m_restartButton->setFixedSize(30, 30);
    m_closeButton->setFixedSize(30, 30);
    m_playPauseButton->setDefault(false);
    m_restartButton->setDefault(false);
    m_closeButton->setDefault(false);
    
    m_playPauseButton->setIcon(QIcon(":/icons/play.svg"));
    m_playPauseButton->setToolTip(tr("开始"));
    m_restartButton->setIcon(QIcon(":/icons/restart.svg"));
    m_restartButton->setToolTip(tr("重置"));
    m_closeButton->setIcon(QIcon(":/icons/quit.svg"));
    m_closeButton->setToolTip(tr("关闭"));
    
    m_playPauseButton->setIconSize(QSize(15, 15));
    m_restartButton->setIconSize(QSize(15, 15));
    m_closeButton->setIconSize(QSize(15, 15));

    m_playPauseButton->setStyleSheet(
        "QPushButton {"
        "    background: none;"
        "    border: none; border-radius:10px; "
        "}"
        "QPushButton:hover {"
        "    background: palette(highlight);"
        "}"
        "QPushButton:pressed {"
        "    background: none;"
        "}"
    );
    
    m_restartButton->setStyleSheet(m_playPauseButton->styleSheet());
    m_closeButton->setStyleSheet(m_playPauseButton->styleSheet());
    
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_playPauseButton);
    buttonLayout->addWidget(m_restartButton);
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_timeLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    showButton(false);
    
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    adjustSize();
    
    // Center on screen
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void TimerWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void TimerWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void TimerWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    raise();
    
    painter.setBrush(QBrush(QColor(0, 0, 0, 150)));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 15, 15);
}

void TimerWindow::closeEvent(QCloseEvent *event)
{
    m_timer->stop();
    event->accept();
}

void TimerWindow::onTimerTimeout()
{
    if(m_started && m_running && !m_saverRunning){
        m_time = m_time.addSecs(1);
    }
#if defined(Q_OS_WIN)
    ::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &m_saverRunning, 0);
    m_saverRunning &= ConfigManager::instance()->smartTimer();
#endif
    updateDisplay();
}

void TimerWindow::onPlayPauseClicked()
{
    if (!m_started) {
        m_started = true;
        m_running = true;
        updatePlayPauseButton();
        return;
    }
    m_running = !m_running;
    updatePlayPauseButton();
}

void TimerWindow::onStopClicked()
{
    m_time.setHMS(0, 0, 0);
    updateDisplay();
    m_started = false;
    m_running = false;
    updatePlayPauseButton();
}

void TimerWindow::onCloseClicked()
{
    close();
}

void TimerWindow::updateDisplay()
{
    m_timeLabel->setText(m_time.toString("HH:mm:ss"));
}

void TimerWindow::updatePlayPauseButton()
{
    if (!m_started) {
        m_playPauseButton->setIcon(QIcon(":/icons/play.svg"));
        m_playPauseButton->setToolTip(tr("开始"));
    } else if (m_running) {
        m_playPauseButton->setIcon(QIcon(":/icons/pause2.svg"));
        m_playPauseButton->setToolTip(tr("暂停"));
    } else {
        m_playPauseButton->setIcon(QIcon(":/icons/play.svg"));
        m_playPauseButton->setToolTip(tr("开始"));
    }
}

bool TimerWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        // 窗口失去焦点时重新激活
        QTimer::singleShot(2000, this, [this]() {
            raise();
            // activateWindow();
        });
    }
    if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverMove) {
        showButton(true);
    } else if (event->type() == QEvent::HoverLeave){
        showButton(false);
    }
    return QWidget::event(event);
}

void TimerWindow::onScreenStateChanged(ScreenState state)
{
    if (!ConfigManager::instance()->smartTimer() || !m_started){
        return;
    }
    // 智能计时模式：锁屏/屏保时暂停，解锁后恢复
    switch (state) {
    case ScreenState::ScreenSaverStart:
    case ScreenState::Locked:
        // pauseTimer(true);
        m_running = false;
        break;
    case ScreenState::ScreenSaverStop:
    case ScreenState::UnlockedFromLock:
    case ScreenState::Unlocked:
        m_running = true;
        // pauseTimer(false);
        break;
    }
}
