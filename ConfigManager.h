#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager* instance();
    
    void loadConfig(bool reload=false);
    void saveConfig();
    
    int workTime() const { return m_workTime; }
    void setWorkTime(int workTime);
    
    int breakTime() const { return m_breakTime; }
    void setBreakTime(int breakTime);
    
    QString message() const { return m_message; }
    void setMessage(const QString& message);
    
    QString backgroundImage() const { return m_backgroundImage; }
    void setBackgroundImage(const QString& backgroundImage);
    
    QString soundFile() const { return m_soundFile; }
    void setSoundFile(const QString& soundFile);
    
    int screenSaverOption() const { return m_screenSaverOption; }
    void setScreenSaverOption(int option);
    
    int lockScreenOption() const { return m_lockScreenOption; }
    void setLockScreenOption(int option);
    
    bool autoStart() const { return m_autoStart; }
    void setAutoStart(bool autoStart);
    
    bool forceRest() const { return m_forceRest; }
    void setForceRest(bool forceRest);
    
    QString fontFamily() const { return m_fontFamily; }
    void setFontFamily(const QString& fontFamily);
    
    int fontSize() const { return m_fontSize; }
    void setFontSize(int fontSize);
    
    int backgroundType() const { return m_backgroundType; }
    void setBackgroundType(int type);
    
    QColor backgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const QColor& color);
    
    int imageDrawMode() const { return m_imageDrawMode; }
    void setImageDrawMode(int mode);
    
    QString timerFontFamily() const { return m_timerFontFamily; }
    void setTimerFontFamily(const QString& fontFamily);
    
    int timerFontSize() const { return m_timerFontSize; }
    void setTimerFontSize(int fontSize);
    
    QColor timerTextColor() const { return m_timerTextColor; }
    void setTimerTextColor(const QColor& color);
    
    QString countdownFontFamily() const { return m_countdownFontFamily; }
    void setCountdownFontFamily(const QString& fontFamily);
    
    int countdownFontSize() const { return m_countdownFontSize; }
    void setCountdownFontSize(int fontSize);
    
    QColor countdownTextColor() const { return m_countdownTextColor; }
    void setCountdownTextColor(const QColor& color);
    
    QString countdownAlertSound() const { return m_countdownAlertSound; }
    void setCountdownAlertSound(const QString& soundFile);

    // 智能计时：锁屏/屏保时自动暂停，解锁后自动继续
    bool smartTimer() const { return m_smartTimer; }
    void setSmartTimer(bool enabled);

signals:
    void configChanged();

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    QString configFilePath() const;
    void setDefaultValues();

    static ConfigManager *m_instance;
    bool loaded{false};
    // 配置项
    int m_workTime;
    int m_breakTime;
    QString m_message;
    QString m_backgroundImage;
    QString m_soundFile;
    int m_screenSaverOption;
    int m_lockScreenOption;
    bool m_autoStart;
    bool m_forceRest;
    QString m_fontFamily;
    int m_fontSize;
    // 0: black, 1: custom color, 2: image
    int m_backgroundType;
    QColor m_backgroundColor;
    // 0: fill, 1: fit, 2: stretch, 3: tile, 4: center
    int m_imageDrawMode;
    
    QString m_timerFontFamily;
    int m_timerFontSize;
    QColor m_timerTextColor;
    
    QString m_countdownFontFamily;
    int m_countdownFontSize;
    QColor m_countdownTextColor;
    QString m_countdownAlertSound;

    bool m_smartTimer;
};

#endif // CONFIGMANAGER_H
