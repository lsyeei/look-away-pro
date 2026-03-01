#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFont>
#include <QComboBox>
#include <QColor>


class ConfigManager;
namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    void loadSettings();
    void saveSettings();

    // QObject interface
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void settingsChanged();

private slots:
    void onOkClicked();
    void onApplyClicked();
    void onCancelClicked();
    void onBrowseBackgroundImage();
    void onBrowseSoundFile();
    void onBackgroundTypeChanged();
    void onSelectBackgroundColor();
    void onPreview();
    void onTimerColorButtonClicked();
    void onCountdownColorButtonClicked();
    void onBrowseCountdownSound();

private:
    void initializeFontComboBox();
    void updateBackgroundControls();
    void updateImagePreview();
    void initializeToolSettings();

    Ui::SettingsDialog *ui;
    ConfigManager* m_config;
    QColor m_timerTextColor;
    QColor m_countdownTextColor;
};

#endif // SETTINGSDIALOG_H
