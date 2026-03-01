#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "ConfigManager.h"
#include "ReminderDialog.h"
#include <QFileDialog>
#include <QFontDatabase>
#include <QDialogButtonBox>
#include <QColorDialog>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , m_config(nullptr)
{
    // Initialize config manager first
    m_config = ConfigManager::instance();
    if (!m_config) {
        qCritical("Failed to get ConfigManager instance in SettingsDialog");
        return;
    }
    
    ui->setupUi(this);

    initializeFontComboBox();

    loadSettings();

    connect(ui->imageRadioButton, &QRadioButton::toggled, this, &SettingsDialog::onBackgroundTypeChanged);
    connect(ui->colorRadioButton, &QRadioButton::toggled, this, &SettingsDialog::onBackgroundTypeChanged);
    connect(ui->backgroundColorButton, &QPushButton::clicked, this, &SettingsDialog::onSelectBackgroundColor);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SettingsDialog::onOkClicked);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);
    connect(ui->browseSoundButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseSoundFile);
    connect(ui->previewButton, &QPushButton::clicked, this, &SettingsDialog::onPreview);
    ui->imagePreviewLabel->installEventFilter(this);
    
    // Connect value changed signals to auto-update m_config
    connect(ui->workTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this](int value) { if (m_config) m_config->setWorkTime(value); });
    connect(ui->breakTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this](int value) { if (m_config) m_config->setBreakTime(value); });
    connect(ui->messageTextEdit, &QTextEdit::textChanged, 
            [this]() { if (m_config) m_config->setMessage(ui->messageTextEdit->toPlainText()); });
    connect(ui->screenSaverComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [this](int index) { if (m_config) m_config->setScreenSaverOption(index); });
    connect(ui->lockScreenComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [this](int index) { if (m_config) m_config->setLockScreenOption(index); });
    connect(ui->autoStartCheckBox, &QCheckBox::stateChanged, 
            [this](int state) { if (m_config) m_config->setAutoStart(state == Qt::Checked); });
    connect(ui->forceRestCheckBox, &QCheckBox::stateChanged, 
            [this](int state) { if (m_config) m_config->setForceRest(state == Qt::Checked); });
    connect(ui->fontComboBox, &QComboBox::currentTextChanged, 
            [this](const QString& text) { if (m_config) m_config->setFontFamily(text); });
    connect(ui->fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this](int value) { if (m_config) m_config->setFontSize(value); });
    connect(ui->imageDrawModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [this](int index) { if (m_config) m_config->setImageDrawMode(index); });
    
    // Connect tool settings signals
    connect(ui->timerFontComboBox, &QComboBox::currentTextChanged,
            [this](const QString& text) { if (m_config) m_config->setTimerFontFamily(text); });
    connect(ui->timerFontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) { if (m_config) m_config->setTimerFontSize(value); });
    connect(ui->countdownFontComboBox, &QComboBox::currentTextChanged,
            [this](const QString& text) { if (m_config) m_config->setCountdownFontFamily(text); });
    connect(ui->countdownFontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int value) { if (m_config) m_config->setCountdownFontSize(value); });
    
    connect(ui->timerColorButton, &QPushButton::clicked, this, &SettingsDialog::onTimerColorButtonClicked);
    connect(ui->countdownColorButton, &QPushButton::clicked, this, &SettingsDialog::onCountdownColorButtonClicked);
    connect(ui->countdownSoundButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseCountdownSound);
}

void SettingsDialog::initializeFontComboBox()
{
    // Get available fonts and add to combo box
    QFontDatabase fontDatabase;
    QStringList fontFamilies = fontDatabase.families();
    ui->fontComboBox->addItems(fontFamilies);
    ui->timerFontComboBox->addItems(fontFamilies);
    ui->countdownFontComboBox->addItems(fontFamilies);

    // Set default font
    int index = ui->fontComboBox->findText("Microsoft YaHei");
    if (index < 0) {
        index = ui->fontComboBox->findText("SimHei");
    }
    if (index < 0) {
        index = ui->fontComboBox->findText("Arial");
    }
    if (index >= 0) {
        ui->fontComboBox->setCurrentIndex(index);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    m_config = ConfigManager::instance();
    m_config->loadConfig();

    ui->workTimeSpinBox->setValue(m_config->workTime());
    ui->breakTimeSpinBox->setValue(m_config->breakTime());
    ui->messageTextEdit->setPlainText(m_config->message());
    ui->soundFileLineEdit->setText(m_config->soundFile());
    ui->screenSaverComboBox->setCurrentIndex(m_config->screenSaverOption());
    ui->lockScreenComboBox->setCurrentIndex(m_config->lockScreenOption());
    ui->autoStartCheckBox->setChecked(m_config->autoStart());
    ui->forceRestCheckBox->setChecked(m_config->forceRest());
    ui->fontComboBox->setCurrentText(m_config->fontFamily());
    ui->fontSizeSpinBox->setValue(m_config->fontSize());
    ui->imageDrawModeComboBox->setCurrentIndex(m_config->imageDrawMode());
    
    // Load tool settings
    ui->timerFontComboBox->setCurrentText(m_config->timerFontFamily());
    ui->timerFontSizeSpinBox->setValue(m_config->timerFontSize());
    ui->countdownFontComboBox->setCurrentText(m_config->countdownFontFamily());
    ui->countdownFontSizeSpinBox->setValue(m_config->countdownFontSize());
    ui->countdownSoundLineEdit->setText(m_config->countdownAlertSound());
    
    // Store colors for button styling
    m_timerTextColor = m_config->timerTextColor();
    m_countdownTextColor = m_config->countdownTextColor();
    
    // Update color buttons
    ui->timerColorButton->setStyleSheet(
        QString("QPushButton { background-color: %1; }").arg(m_timerTextColor.name())
    );
    ui->countdownColorButton->setStyleSheet(
        QString("QPushButton { background-color: %1; }").arg(m_countdownTextColor.name())
    );
    
    // Set radio buttons based on background type
    int backgroundType = m_config->backgroundType();
    if (backgroundType == 2) {
        ui->imageRadioButton->setChecked(true);
    } else if (backgroundType == 1) {
        ui->colorRadioButton->setChecked(true);
    } else {
        ui->imageRadioButton->setChecked(false);
        ui->colorRadioButton->setChecked(false);
    }
    
    updateBackgroundControls();
    
    // Update previews
    if (backgroundType == 2) {
        updateImagePreview();
    } else if (backgroundType == 1 && m_config->backgroundColor().isValid()) {
        ui->backgroundColorButton->setStyleSheet(
            QString("QPushButton { background-color: %1; }").arg(m_config->backgroundColor().name())
            );
    }
}

void SettingsDialog::saveSettings()
{
    m_config->saveConfig();
    emit settingsChanged();
}

void SettingsDialog::onOkClicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::onApplyClicked()
{
    saveSettings();
}

void SettingsDialog::onCancelClicked()
{
    m_config->loadConfig(true);
    reject();
}

void SettingsDialog::onBrowseBackgroundImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择背景图片",
        "",
        "图片文件 (*.bmp *.jpg *.jpeg *.png *.gif);;所有文件 (*.*)"
    );

    if (!fileName.isEmpty()) {
        m_config->setBackgroundImage(fileName);
        updateImagePreview();

        m_config->setBackgroundImage(fileName);
    }
}

void SettingsDialog::onBrowseSoundFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择提示音",
        "",
        "音频文件 (*.wav *.mp3 *.ogg);;所有文件 (*.*)"
    );

    if (!fileName.isEmpty()) {
        m_config->setSoundFile(fileName);
        ui->soundFileLineEdit->setText(fileName);

        m_config->setSoundFile(fileName);
    }
}

void SettingsDialog::onBackgroundTypeChanged()
{
    auto backgroundType = 0;
    if (ui->imageRadioButton->isChecked()) {
        backgroundType = 2;
    } else if (ui->colorRadioButton->isChecked()) {
        backgroundType = 1;
    } else {
        backgroundType = 0;
    }

    m_config->setBackgroundType(backgroundType);

    updateBackgroundControls();
}

void SettingsDialog::onSelectBackgroundColor()
{
    QColor color = QColorDialog::getColor(m_config->backgroundColor(), this, "选择背景颜色");

    if (color.isValid()) {
        m_config->setBackgroundColor(color);
        // Update button background to show selected color
        ui->backgroundColorButton->setStyleSheet(
            QString("QPushButton { background-color: %1; }").arg(color.name())
        );

        m_config->setBackgroundColor(color);
    }
}

void SettingsDialog::updateBackgroundControls()
{
    bool showImageControls = (m_config->backgroundType() == 2);  // image type
    bool showColorControls = (m_config->backgroundType() == 1);  // custom color type

    ui->imageConfigWidget->setVisible(showImageControls);
    ui->colorConfigWidget->setVisible(showColorControls);

    if (showImageControls) {
        updateImagePreview();
    }

    if (showColorControls) {
        ui->backgroundColorButton->setStyleSheet(
            QString("QPushButton { background-color: %1; }").arg(m_config->backgroundColor().name())
        );
    } else {
        ui->backgroundColorButton->setStyleSheet("");
    }
}

void SettingsDialog::updateImagePreview()
{
    if (!m_config->backgroundImage().isEmpty() &&
        QFile::exists(m_config->backgroundImage())) {
        QPixmap pixmap(m_config->backgroundImage());
        if (!pixmap.isNull()) {
            // Scale pixmap to fit the label while keeping aspect ratio
            QPixmap scaledPixmap = pixmap.scaled(
                ui->imagePreviewLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
            ui->imagePreviewLabel->setPixmap(scaledPixmap);
        } else {
            ui->imagePreviewLabel->setText("无法加载图片");
        }
    } else {
        ui->imagePreviewLabel->setText("浏览...");
    }
}

void SettingsDialog::onPreview()
{
    auto *previewDialog = new ReminderDialog(this);
    previewDialog->showFullScreen();
    previewDialog->preview();
    // Clean up
    delete previewDialog;
}


bool SettingsDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->imagePreviewLabel && event->type() == QEvent::MouseButtonPress) {
        onBrowseBackgroundImage();
    }
    return QWidget::eventFilter(watched, event);
}

void SettingsDialog::onTimerColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_timerTextColor, this, "选择计时器文字颜色");
    
    if (color.isValid()) {
        m_timerTextColor = color;
        m_config->setTimerTextColor(color);
        ui->timerColorButton->setStyleSheet(
            QString("QPushButton { background-color: %1; }").arg(color.name())
        );
    }
}

void SettingsDialog::onCountdownColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_countdownTextColor, this, "选择倒计时器文字颜色");
    
    if (color.isValid()) {
        m_countdownTextColor = color;
        m_config->setCountdownTextColor(color);
        ui->countdownColorButton->setStyleSheet(
            QString("QPushButton { background-color: %1; }").arg(color.name())
        );
    }
}

void SettingsDialog::onBrowseCountdownSound()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择倒计时提示音",
        "",
        "音频文件 (*.wav *.mp3);;所有文件 (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        ui->countdownSoundLineEdit->setText(fileName);
        m_config->setCountdownAlertSound(fileName);
    }
}
