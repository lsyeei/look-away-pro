#include "ReminderDialog.h"
#include "AudioPlayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QPixmap>
#include <QFile>
#include <QUrl>
#include <QKeyEvent>
#include <QApplication>

ReminderDialog::ReminderDialog(QWidget *parent)
    : QDialog(parent)
    , m_messageLabel(nullptr)
    , m_countdownLabel(nullptr)
    , m_closeButton(nullptr)
    , m_remainingTime(20)
    , m_config(nullptr)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Dialog | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_config = ConfigManager::instance();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    m_messageLabel = new QLabel(this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // mainLayout->addStretch();
    mainLayout->addWidget(m_messageLabel, 0, Qt::AlignCenter);

    m_countdownLabel = new QLabel(this);
    m_countdownLabel->setAlignment(Qt::AlignCenter);
    m_countdownLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainLayout->addWidget(m_countdownLabel, 0, Qt::AlignCenter);

    m_closeButton = new QPushButton("关闭", this);
    m_closeButton->setFixedSize(120, 40);
    m_closeButton->setStyleSheet("QPushButton { font-size: 16px; font-weight: bold; "
                                 "background-color: rgba(100, 100, 100, 200); "
                                 "color: white; border: none; border-radius: 5px; } "
                                 "QPushButton:hover { "
                                 "background-color: rgba(220, 120, 120, 200); }");
    m_closeButton->setVisible(false);
    connect(m_closeButton, &QPushButton::clicked, this, [&](){
        m_countdownTimer.stop();
        accept();});
    mainLayout->addWidget(m_closeButton, 0, Qt::AlignCenter);

    connect(&m_countdownTimer, &QTimer::timeout, this, &ReminderDialog::updateCountdown);
    updateMessageFont();
}

void ReminderDialog::preview()
{
    m_closeButton->setVisible(true);
    m_countdownLabel->setVisible(true);
    auto imgFile = m_config->backgroundImage();
    if(m_config->backgroundType() == 2 && !imgFile.isEmpty()){
        m_backgroundPixmap.load(imgFile);
    }
    updateMessageFont();
    QString message = m_config->message();
    message.replace("{time}", QString::number(m_config->workTime()));
    message.replace("{break}", QString::number(m_config->breakTime()));
    m_messageLabel->setText(message);
    m_messageLabel->adjustSize();
    QFontMetrics fm(m_messageLabel->font());
    QRect textRect = fm.boundingRect(QRect(0, 0, m_messageLabel->width(), 0),
                                     Qt::TextWordWrap | Qt::AlignCenter,
                                     m_messageLabel->text());
    m_messageLabel->setFixedHeight(textRect.height() + 5);
    m_countdownLabel->setText(QString("剩余时间: %1 秒").arg(m_config->breakTime()));

    QDialog::exec();
}

void ReminderDialog::updateMessageFont()
{
    QFont font(m_config->fontFamily(), m_config->fontSize(), QFont::Bold);
    m_messageLabel->setFont(font);
    int size = font.pointSize()*3/4;
    if (size <= 10) {
        size = 10;
    }
    m_countdownLabel->setFont(QFont{m_config->fontFamily(), size});
}
void ReminderDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto imgRect = m_backgroundPixmap.rect();
    auto winRect = rect();
    // Draw background based on image draw mode
    if (m_config->backgroundType() == 2 && !m_backgroundPixmap.isNull()) {
        switch (m_config->imageDrawMode()) {
        case 0:  // Fill (KeepAspectRatioByExpanding)
            painter.drawPixmap(rect(), m_backgroundPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            break;
        case 1:  // Fit (KeepAspectRatio)
            painter.drawPixmap(rect(), m_backgroundPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            break;
        case 2:  // Stretch (IgnoreAspectRatio)
            painter.drawPixmap(rect(), m_backgroundPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            break;
        case 3:  // Tile
            painter.drawTiledPixmap(rect(), m_backgroundPixmap);
            break;
        case 4:  // center
            painter.fillRect(rect(),Qt::black);
            imgRect.moveCenter(winRect.center());
            painter.drawPixmap(imgRect, m_backgroundPixmap);
            break;
        default:  // Default: fit
            painter.drawPixmap(rect(), m_backgroundPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            break;
        }
    } else if (m_config->backgroundType() == 1 && m_config->backgroundColor().isValid()){
        // Solid color or gradient background
        painter.fillRect(rect(), m_config->backgroundColor());
    } else {
        // Default black background
        painter.fillRect(rect(), QColor(0, 0, 0));
    }
}

void ReminderDialog::keyPressEvent(QKeyEvent *event)
{
    if (m_config->forceRest()) {
        event->ignore();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void ReminderDialog::closeEvent(QCloseEvent *event)
{
    // Allow closing only if break time is finished
    if (m_remainingTime > 0 && m_config->forceRest()) {
        event->ignore();
    } else {
        QDialog::closeEvent(event);
    }
}

int ReminderDialog::exec()
{
    m_closeButton->setVisible(!m_config->forceRest());
    auto imgFile = m_config->backgroundImage();
    if(m_config->backgroundType() == 2 && !imgFile.isEmpty()){
        m_backgroundPixmap.load(imgFile);
    }
    updateMessageFont();

    QString message = m_config->message();
    message.replace("{time}", QString::number(m_config->workTime()));
    message.replace("{break}", QString::number(m_config->breakTime()));
    m_messageLabel->setText(message);
    m_messageLabel->adjustSize();
    QFontMetrics fm(m_messageLabel->font());
    QRect textRect = fm.boundingRect(QRect(0, 0, m_messageLabel->width(), 0),
                                     Qt::TextWordWrap,
                                     m_messageLabel->text());
    m_messageLabel->setFixedHeight(textRect.height() + 5);

    m_remainingTime = m_config->breakTime();

    m_countdownLabel->setText(QString("剩余时间: %1 秒").arg(m_remainingTime));
    m_countdownTimer.start(1000);

    return QDialog::exec();
}

void ReminderDialog::updateCountdown()
{
    m_remainingTime--;

    if (m_remainingTime >= 0) {
        m_countdownLabel->setText(QString("剩余时间: %1 秒").arg(m_remainingTime));
    }

    if (m_remainingTime <= 0) {
        onBreakFinished();
    }
}

void ReminderDialog::onBreakFinished()
{
    m_countdownTimer.stop();

    auto soundFile = m_config->soundFile();
    if (soundFile.isEmpty()) {
        soundFile = "qrc:/sound/break-end.wav";
    }
    AudioPlayer::instance()->play(soundFile);

    // Always close dialog when break finishes regardless of forceRest setting
    accept();
}

bool ReminderDialog::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        // 窗口失去焦点时重新激活
        QTimer::singleShot(100, this, [this]() {
            raise();
            activateWindow();
        });
    }
    return QDialog::event(event);
}