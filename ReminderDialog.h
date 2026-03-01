#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include "ConfigManager.h"

#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPixmap>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QColor>

class ReminderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReminderDialog(QWidget *parent = nullptr);

    int exec() override;
    void preview();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateCountdown();
    void onBreakFinished();

private:
    void updateMessageFont();
    QLabel *m_messageLabel;
    QLabel *m_countdownLabel;
    QPushButton *m_closeButton;
    QTimer m_countdownTimer;

    QPixmap m_backgroundPixmap;
    int m_remainingTime;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    ConfigManager *m_config;
};

#endif // REMINDERDIALOG_H
