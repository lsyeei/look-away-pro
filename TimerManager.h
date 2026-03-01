#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <QObject>
#include <QTimer>

class TimerManager : public QObject
{
    Q_OBJECT

public:
    explicit TimerManager(QObject *parent = nullptr);

    void setWorkTime(int minutes);
    void setBreakTime(int seconds);
    void start();
    void pause();
    void resume();
    void stop();
    void reset();

    int getRemainingTime() const;
    bool isRunning() const;

signals:
    void timerComplete();
    void sessionStart();
    void sessionEnd();

private slots:
    void onTimerTick();
    void onWorkTimerTimeout();

private:
    QTimer m_workTimer;
    QTimer m_countdownTimer;

    int m_workTime;       // in minutes
    int m_breakTime;      // in seconds
    int m_remainingTime;  // in seconds
    bool m_running;
};

#endif // TIMERMANAGER_H
