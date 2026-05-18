#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <QObject>
#include <QTimer>

class TimerManager : public QObject
{
    Q_OBJECT

public:
    explicit TimerManager(QObject *parent = nullptr);
    ~TimerManager();
    void setWorkTime(int minutes);
    void setBreakTime(int seconds);
    void start();
    void stop();
    void reset();
    void setAutoFlag(bool flag);
    void setScreenLocked(bool flag);

    int getRemainingTime() const;

signals:
    void lookAwayTrigger();
    void workStart();
    void workEnd();

private slots:
    void onTimerTick();

private:
    QTimer m_timer;

    int m_workTime;       // in minutes
    int m_breakTime;      // in seconds
    int m_remainingTime;  // in seconds
    bool m_running;
    // 自动运行标记，可通过菜单控制，true auto，false manual
    bool m_autoFlag;
    // 屏保运行标记
    bool m_saverRunning;
    // 锁屏标记
    bool m_screenLocked;
};

#endif // TIMERMANAGER_H
