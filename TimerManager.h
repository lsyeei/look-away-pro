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
    void pause();
    void resume();
    void stop();
    void reset();
    void setAutoFlag(bool flag);

    int getRemainingTime() const;
    bool isRunning() const;

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
    bool m_saverRunning;
};

#endif // TIMERMANAGER_H
