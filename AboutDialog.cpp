#include "AboutDialog.h"
#include <QApplication>
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    
    // Make window frameless
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    
    // Set fixed size
    setFixedSize(400, 300);
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setupUI()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(10);
    
    // Application icon
    m_iconLabel = new QLabel(this);
    QPixmap iconPixmap = QPixmap(":/icon.ico").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_iconLabel->setPixmap(iconPixmap);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_iconLabel);
    
    // Application name
    m_titleLabel = new QLabel("Look Away Pro", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    m_mainLayout->addWidget(m_titleLabel);
    
    // Version
    m_versionLabel = new QLabel("版本: 1.0", this);
    m_versionLabel->setAlignment(Qt::AlignCenter);
    m_versionLabel->setStyleSheet("font-size: 14px; color: white;");
    m_mainLayout->addWidget(m_versionLabel);
    
    // Spacer
    m_mainLayout->addSpacing(10);
    
    // Author email (clickable)
    m_emailLabel = new QLabel("作者邮箱: <a href='mailto:lsyeei@163.com' style='color: #4CAF50;'>lsyeei@163.com</a>", this);
    m_emailLabel->setAlignment(Qt::AlignCenter);
    m_emailLabel->setOpenExternalLinks(true);
    m_emailLabel->setStyleSheet("font-size: 13px; color: white;");
    m_mainLayout->addWidget(m_emailLabel);
    
    // Project link (clickable)
    m_projectLabel = new QLabel("项目地址: <a href='https://github.com/lsyeei/look-away-pro' style='color: #4CAF50;'>https://github.com/lsyeei/look-away-pro</a>", this);
    m_projectLabel->setAlignment(Qt::AlignCenter);
    m_projectLabel->setOpenExternalLinks(true);
    m_projectLabel->setStyleSheet("font-size: 13px; color: white;");
    m_mainLayout->addWidget(m_projectLabel);
    
    // Add stretch to push button to bottom
    m_mainLayout->addStretch();
    
    // Close button
    m_closeButton = new QPushButton("关闭", this);
    m_closeButton->setFixedSize(100, 35);
    m_closeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
    );
    connect(m_closeButton, &QPushButton::clicked, this, &AboutDialog::accept);
    
    // Center the button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();
    m_mainLayout->addLayout(buttonLayout);
}

void AboutDialog::mousePressEvent(QMouseEvent *event)
{
    // Close dialog when clicking outside
    if (!rect().contains(event->pos())) {
        accept();
    } else {
        QDialog::mousePressEvent(event);
    }
}

void AboutDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw semi-transparent background with blur effect
    painter.fillRect(rect(), QColor(0, 0, 0, 200));
    
    // Draw main dialog background
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 15, 15);
    painter.fillPath(path, QColor(50, 50, 50, 240));
    
    // Draw border
    painter.setPen(QColor(100, 100, 100, 150));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 15, 15);
}
