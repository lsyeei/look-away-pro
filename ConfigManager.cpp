#include "ConfigManager.h"
#include <QCoreApplication>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>

ConfigManager* ConfigManager::m_instance = nullptr;

ConfigManager* ConfigManager::instance()
{
    if (!m_instance) {
        m_instance = new ConfigManager();
    }
    return m_instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    // Initialize all member variables with defaults
    setDefaultValues();
    loaded = false;
}

ConfigManager::~ConfigManager()
{
    // Don't save config in destructor - it may be called after QCoreApplication is destroyed
    // saveConfig();
}

void ConfigManager::setDefaultValues()
{
    m_workTime = 20;
    m_breakTime = 20;
    m_message = "您已工作{time}分钟，远眺20英尺外{break}秒，\n请保护好眼睛";
    m_backgroundImage = "";
    m_soundFile = "";
    m_screenSaverOption = 0;
    m_lockScreenOption = 0;
    m_autoStart = false;
    m_forceRest = true;
    m_fontFamily = "Microsoft YaHei";
    m_fontSize = 24;
    m_backgroundType = 0;
    m_backgroundColor = QColor(0, 0, 0);
    m_imageDrawMode = 1;
    
    m_timerFontFamily = "Arial";
    m_timerFontSize = 24;
    m_timerTextColor = QColor(255, 255, 255);
    
    m_countdownFontFamily = "Arial";
    m_countdownFontSize = 24;
    m_countdownTextColor = QColor(255, 255, 255);
    m_countdownAlertSound = "";
}

QString ConfigManager::configFilePath() const
{
    return QCoreApplication::applicationDirPath() + "/config.xml";
}

void ConfigManager::loadConfig(bool reload)
{
    if (!reload && loaded) {
        return;
    }
    
    QString configPath = configFilePath();
    QFile file(configPath);
    
    // Check if config file exists
    if (!file.exists()) {
        qDebug() << "Config file does not exist:" << configPath;
        loaded = true;  // Mark as loaded to avoid repeated attempts
        return;
    }
    
    // Try to open config file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open config file for reading:" << configPath 
                   << "Error:" << file.errorString();
        loaded = true;  // Mark as loaded to avoid repeated attempts
        return;
    }
    
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qWarning() << "Invalid XML content in config file:" << configPath;
        file.close();
        loaded = true;  // Mark as loaded to avoid repeated attempts
        return;
    }
    file.close();
    
    QDomElement root = doc.documentElement();
    if (root.tagName() != "config") {
        qWarning() << "Invalid root element in config file:" << root.tagName();
        loaded = true;  // Mark as loaded to avoid repeated attempts
        return;
    }
    
    QDomElement settings = root.firstChildElement("settings");
    if (settings.isNull()) {
        qWarning() << "No settings element found in config file";
        loaded = true;  // Mark as loaded to avoid repeated attempts
        return;
    }
    
    // Load all settings with validation
    m_workTime = qMax(1, settings.firstChildElement("workTime").text().toInt());
    m_breakTime = qMax(1, settings.firstChildElement("breakTime").text().toInt());
    m_message = settings.firstChildElement("message").text();
    m_backgroundImage = settings.firstChildElement("backgroundImage").text();
    m_soundFile = settings.firstChildElement("soundFile").text();
    m_screenSaverOption = qBound(0, settings.firstChildElement("screenSaverOption").text().toInt(), 2);
    m_lockScreenOption = qBound(0, settings.firstChildElement("lockScreenOption").text().toInt(), 2);
    m_autoStart = settings.firstChildElement("autoStart").text().toInt() == 1;
    m_forceRest = settings.firstChildElement("forceRest").text().toInt() == 1;
    m_fontFamily = settings.firstChildElement("fontFamily").text();
    if (m_fontFamily.isEmpty()) {
        m_fontFamily = "Microsoft YaHei";
    }
    m_fontSize = qBound(8, settings.firstChildElement("fontSize").text().toInt(), 72);
    m_backgroundType = qBound(0, settings.firstChildElement("backgroundType").text().toInt(), 2);
    m_backgroundColor = QColor(settings.firstChildElement("backgroundColor").text());
    if (!m_backgroundColor.isValid()) {
        m_backgroundColor = QColor(0, 0, 0);
    }
    m_imageDrawMode = qBound(0, settings.firstChildElement("imageDrawMode").text().toInt(), 3);
    
    m_timerFontFamily = settings.firstChildElement("timerFontFamily").text();
    if (m_timerFontFamily.isEmpty()) {
        m_timerFontFamily = "Arial";
    }
    m_timerFontSize = qBound(8, settings.firstChildElement("timerFontSize").text().toInt(), 72);
    m_timerTextColor = QColor(settings.firstChildElement("timerTextColor").text());
    if (!m_timerTextColor.isValid()) {
        m_timerTextColor = QColor(255, 255, 255);
    }
    
    m_countdownFontFamily = settings.firstChildElement("countdownFontFamily").text();
    if (m_countdownFontFamily.isEmpty()) {
        m_countdownFontFamily = "Arial";
    }
    m_countdownFontSize = qBound(8, settings.firstChildElement("countdownFontSize").text().toInt(), 72);
    m_countdownTextColor = QColor(settings.firstChildElement("countdownTextColor").text());
    if (!m_countdownTextColor.isValid()) {
        m_countdownTextColor = QColor(255, 255, 255);
    }
    m_countdownAlertSound = settings.firstChildElement("countdownAlertSound").text();
    
    emit configChanged();
    loaded = true;
    qDebug() << "Config loaded successfully from:" << configPath;
}

void ConfigManager::saveConfig()
{
    QString configPath = configFilePath();
    QDomDocument doc;
    
    QDomElement root = doc.createElement("config");
    doc.appendChild(root);
    
    QDomElement settings = doc.createElement("settings");
    root.appendChild(settings);
    
    QDomElement workTime = doc.createElement("workTime");
    workTime.appendChild(doc.createTextNode(QString::number(m_workTime)));
    settings.appendChild(workTime);
    
    QDomElement breakTime = doc.createElement("breakTime");
    breakTime.appendChild(doc.createTextNode(QString::number(m_breakTime)));
    settings.appendChild(breakTime);
    
    QDomElement message = doc.createElement("message");
    message.appendChild(doc.createTextNode(m_message));
    settings.appendChild(message);
    
    QDomElement backgroundImage = doc.createElement("backgroundImage");
    backgroundImage.appendChild(doc.createTextNode(m_backgroundImage));
    settings.appendChild(backgroundImage);
    
    QDomElement soundFile = doc.createElement("soundFile");
    soundFile.appendChild(doc.createTextNode(m_soundFile));
    settings.appendChild(soundFile);
    
    QDomElement screenSaverOption = doc.createElement("screenSaverOption");
    screenSaverOption.appendChild(doc.createTextNode(QString::number(m_screenSaverOption)));
    settings.appendChild(screenSaverOption);
    
    QDomElement lockScreenOption = doc.createElement("lockScreenOption");
    lockScreenOption.appendChild(doc.createTextNode(QString::number(m_lockScreenOption)));
    settings.appendChild(lockScreenOption);
    
    QDomElement autoStart = doc.createElement("autoStart");
    autoStart.appendChild(doc.createTextNode(m_autoStart ? "1" : "0"));
    settings.appendChild(autoStart);
    
    QDomElement forceRest = doc.createElement("forceRest");
    forceRest.appendChild(doc.createTextNode(m_forceRest ? "1" : "0"));
    settings.appendChild(forceRest);
    
    QDomElement fontFamily = doc.createElement("fontFamily");
    fontFamily.appendChild(doc.createTextNode(m_fontFamily));
    settings.appendChild(fontFamily);
    
    QDomElement fontSize = doc.createElement("fontSize");
    fontSize.appendChild(doc.createTextNode(QString::number(m_fontSize)));
    settings.appendChild(fontSize);
    
    QDomElement backgroundType = doc.createElement("backgroundType");
    backgroundType.appendChild(doc.createTextNode(QString::number(m_backgroundType)));
    settings.appendChild(backgroundType);
    
    QDomElement backgroundColor = doc.createElement("backgroundColor");
    backgroundColor.appendChild(doc.createTextNode(m_backgroundColor.name()));
    settings.appendChild(backgroundColor);
    
    QDomElement imageDrawMode = doc.createElement("imageDrawMode");
    imageDrawMode.appendChild(doc.createTextNode(QString::number(m_imageDrawMode)));
    settings.appendChild(imageDrawMode);
    
    QDomElement timerFontFamily = doc.createElement("timerFontFamily");
    timerFontFamily.appendChild(doc.createTextNode(m_timerFontFamily));
    settings.appendChild(timerFontFamily);
    
    QDomElement timerFontSize = doc.createElement("timerFontSize");
    timerFontSize.appendChild(doc.createTextNode(QString::number(m_timerFontSize)));
    settings.appendChild(timerFontSize);
    
    QDomElement timerTextColor = doc.createElement("timerTextColor");
    timerTextColor.appendChild(doc.createTextNode(m_timerTextColor.name()));
    settings.appendChild(timerTextColor);
    
    QDomElement countdownFontFamily = doc.createElement("countdownFontFamily");
    countdownFontFamily.appendChild(doc.createTextNode(m_countdownFontFamily));
    settings.appendChild(countdownFontFamily);
    
    QDomElement countdownFontSize = doc.createElement("countdownFontSize");
    countdownFontSize.appendChild(doc.createTextNode(QString::number(m_countdownFontSize)));
    settings.appendChild(countdownFontSize);
    
    QDomElement countdownTextColor = doc.createElement("countdownTextColor");
    countdownTextColor.appendChild(doc.createTextNode(m_countdownTextColor.name()));
    settings.appendChild(countdownTextColor);
    
    QDomElement countdownAlertSound = doc.createElement("countdownAlertSound");
    countdownAlertSound.appendChild(doc.createTextNode(m_countdownAlertSound));
    settings.appendChild(countdownAlertSound);
    
    QFile file(configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        doc.save(stream, 4);
        file.close();
    }
    emit configChanged();
}

void ConfigManager::setWorkTime(int workTime)
{
    if (m_workTime != workTime) {
        m_workTime = workTime;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setBreakTime(int breakTime)
{
    if (m_breakTime != breakTime) {
        m_breakTime = breakTime;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setMessage(const QString& message)
{
    if (m_message != message) {
        m_message = message;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setBackgroundImage(const QString& backgroundImage)
{
    if (m_backgroundImage != backgroundImage) {
        m_backgroundImage = backgroundImage;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setSoundFile(const QString& soundFile)
{
    if (m_soundFile != soundFile) {
        m_soundFile = soundFile;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setScreenSaverOption(int option)
{
    if (m_screenSaverOption != option) {
        m_screenSaverOption = option;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setLockScreenOption(int option)
{
    if (m_lockScreenOption != option) {
        m_lockScreenOption = option;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setAutoStart(bool autoStart)
{
    if (m_autoStart != autoStart) {
        m_autoStart = autoStart;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setForceRest(bool forceRest)
{
    if (m_forceRest != forceRest) {
        m_forceRest = forceRest;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setFontFamily(const QString& fontFamily)
{
    if (m_fontFamily != fontFamily) {
        m_fontFamily = fontFamily;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setFontSize(int fontSize)
{
    if (m_fontSize != fontSize) {
        m_fontSize = fontSize;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setBackgroundType(int type)
{
    if (m_backgroundType != type) {
        m_backgroundType = type;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setBackgroundColor(const QColor& color)
{
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setImageDrawMode(int mode)
{
    if (m_imageDrawMode != mode) {
        m_imageDrawMode = mode;
        // saveConfig();
        emit configChanged();
    }
}

void ConfigManager::setTimerFontFamily(const QString& fontFamily)
{
    if (m_timerFontFamily != fontFamily) {
        m_timerFontFamily = fontFamily;
        emit configChanged();
    }
}

void ConfigManager::setTimerFontSize(int fontSize)
{
    if (m_timerFontSize != fontSize) {
        m_timerFontSize = fontSize;
        emit configChanged();
    }
}

void ConfigManager::setTimerTextColor(const QColor& color)
{
    if (m_timerTextColor != color) {
        m_timerTextColor = color;
        emit configChanged();
    }
}

void ConfigManager::setCountdownFontFamily(const QString& fontFamily)
{
    if (m_countdownFontFamily != fontFamily) {
        m_countdownFontFamily = fontFamily;
        emit configChanged();
    }
}

void ConfigManager::setCountdownFontSize(int fontSize)
{
    if (m_countdownFontSize != fontSize) {
        m_countdownFontSize = fontSize;
        emit configChanged();
    }
}

void ConfigManager::setCountdownTextColor(const QColor& color)
{
    if (m_countdownTextColor != color) {
        m_countdownTextColor = color;
        emit configChanged();
    }
}

void ConfigManager::setCountdownAlertSound(const QString& soundFile)
{
    if (m_countdownAlertSound != soundFile) {
        m_countdownAlertSound = soundFile;
        emit configChanged();
    }
}
