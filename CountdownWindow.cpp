#include "CountdownWindow.h"
#include "ConfigManager.h"
#include "AudioPlayer.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>
#include <QDebug>
#include <QApplication>
#include <QPropertyAnimation>
#include <QtConcurrent>

CountdownWindow::CountdownWindow(int hours, int minutes, int seconds, QWidget *parent)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_time(hours, minutes, seconds)
    , m_timeLabel(new QLabel(this))
    , m_closeButton(new QPushButton(this))
    , m_restartBtn(new QPushButton(this))
    , m_finished(false)
    , m_flashCount(0)
{
    setupUI();
    m_initTime = m_time;
    
    connect(m_timer, &QTimer::timeout, this, &CountdownWindow::onTimerTimeout);
    connect(m_closeButton, &QPushButton::clicked, this, &CountdownWindow::onCloseClicked);
    connect(m_restartBtn, &QPushButton::clicked, this, &CountdownWindow::onRestartClicked);
    connect(ConfigManager::instance(), &ConfigManager::configChanged,
            this, [&](){updateTimeStyle();});
    
    m_timer->start(1000);
    updateDisplay();
}

CountdownWindow::~CountdownWindow()
{
}

void CountdownWindow::updateTimeStyle()
{
    auto config = ConfigManager::instance();
    QString fontFamily = config->countdownFontFamily();
    int fontSize = config->countdownFontSize();
    QColor textColor = config->countdownTextColor();

    if (fontFamily.isEmpty() || fontSize <= 0) {
        fontFamily = "Arial";
        fontSize = 24;
        textColor = Qt::white;
    }

    QFont font(fontFamily, fontSize);
    font.setBold(true);
    m_timeLabel->setFont(font);
    m_timeLabel->setText(m_time.toString("HH:mm:ss"));
    m_timeLabel->setStyleSheet(QString("color: %1;").arg(textColor.name()));
}

void CountdownWindow::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Ensure minimum size
    setMinimumSize(200, 100);
    
    updateTimeStyle();
    m_timeLabel->setAlignment(Qt::AlignCenter);
    
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setIcon(QIcon(":/icons/quit.svg"));
    m_closeButton->setIconSize(QSize(15, 15));

    /*m_closeButton->*/setStyleSheet(
        "QPushButton {"
        "    background: none;"
        "    border: none; border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "    background: palette(highlight);"
        "}"
        "QPushButton:pressed {"
        "    background: none;"
        "}"
    );

    m_restartBtn->setFixedSize(30, 30);
    m_restartBtn->setIcon(QIcon(":/icons/restart.svg"));
    m_restartBtn->setIconSize(QSize(15, 15));
    m_restartBtn->hide();
    
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_restartBtn);
    buttonLayout->addWidget(m_closeButton);
    // buttonLayout->setContentsMargins(0, 0, 10, 0);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_timeLabel);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    adjustSize();
    
    // Center on screen
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void CountdownWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void CountdownWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void CountdownWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_finished && m_flashCount % 2 == 1) {
        painter.setBrush(QBrush(QColor(255, 0, 0, 150)));
    } else {
        painter.setBrush(QBrush(QColor(0, 0, 0, 150)));
    }
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 15, 15);
}

void CountdownWindow::closeEvent(QCloseEvent *event)
{
    m_timer->stop();
    event->accept();
}

void CountdownWindow::onTimerTimeout()
{
    m_time = m_time.addSecs(-1);
    updateDisplay();
    
    if (m_time.hour() == 0 && m_time.minute() == 0 && m_time.second() == 0) {
        m_timer->stop();
        m_finished = true;
        auto future = QtConcurrent::run([&](){playAlertSound();});
        flashWindow();
    }
}

void CountdownWindow::onCloseClicked()
{
    AudioPlayer::instance()->stop();
    m_timer->stop();
    close();
}

void CountdownWindow::onRestartClicked()
{
    AudioPlayer::instance()->stop();
    m_time = m_initTime;
    m_restartBtn->hide();
    m_timer->start(1000);
    updateDisplay();
}

void CountdownWindow::onFinished()
{
    m_flashCount++;
    update();
    
    if (m_flashCount < 10) {
        QTimer::singleShot(300, this, &CountdownWindow::onFinished);
    } else {
        // close();
        m_restartBtn->show();
    }
}

void CountdownWindow::updateDisplay()
{
    m_timeLabel->setText(m_time.toString("HH:mm:ss"));
}

void CountdownWindow::playAlertSound()
{
    auto config = ConfigManager::instance();
    QString soundFile = config->countdownAlertSound();

    if (soundFile.isEmpty()) {
        soundFile = "qrc:/sound/time-end.wav";
    }
    AudioPlayer::instance()->play(soundFile);
}

void CountdownWindow::flashWindow()
{
    m_flashCount = 0;
    onFinished();
}
