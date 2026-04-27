#ifndef TIMERWINDOW_H
#define TIMERWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QLabel>

class TimerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TimerWindow(QWidget *parent = nullptr);
    ~TimerWindow();

    // QObject interface
    bool event(QEvent *event) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTimerTimeout();
    void onPlayPauseClicked();
    void onStopClicked();
    void onCloseClicked();

private:
    void setupUI();
    void updateDisplay();
    void updatePlayPauseButton();
    void updateTimeStyle();
    void showButton(bool flag);

    QTimer *m_timer;
    QTime m_time;
    QLabel *m_timeLabel;
    QPushButton *m_playPauseButton;
    QPushButton *m_restartButton;
    QPushButton *m_closeButton;
    
    bool m_running;
    bool m_started;
    QPoint m_dragPosition;
};

#endif // TIMERWINDOW_H
