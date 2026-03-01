#ifndef COUNTDOWNWINDOW_H
#define COUNTDOWNWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QLabel>

class QAudioOutput;
class QMediaPlayer;
class CountdownWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CountdownWindow(int hours, int minutes, int seconds, QWidget *parent = nullptr);
    ~CountdownWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTimerTimeout();
    void onCloseClicked();
    void onFinished();

private:
    void setupUI();
    void updateDisplay();
    void playAlertSound();
    void flashWindow();

    QTimer *m_timer;
    QTime m_time;
    QLabel *m_timeLabel;
    QPushButton *m_closeButton;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    
    bool m_finished;
    QPoint m_dragPosition;
    int m_flashCount;
    void updateTimeStyle();
};

#endif // COUNTDOWNWINDOW_H
