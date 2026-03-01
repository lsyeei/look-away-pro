#include "TimerManager.h"

TimerManager::TimerManager(QObject *parent)
    : QObject(parent)
    , m_workTime(20)
    , m_breakTime(20)
    , m_remainingTime(20 * 60)
    , m_running(false)
{
    m_workTimer.setSingleShot(true);
    connect(&m_workTimer, &QTimer::timeout, this, &TimerManager::onWorkTimerTimeout);

    m_countdownTimer.setInterval(1000);
    connect(&m_countdownTimer, &QTimer::timeout, this, &TimerManager::onTimerTick);
}

void TimerManager::setWorkTime(int minutes)
{
    m_workTime = minutes;
    if (!m_running) {
        m_remainingTime = minutes * 60;
    }
}

void TimerManager::setBreakTime(int seconds)
{
    m_breakTime = seconds;
}

void TimerManager::start()
{
    if (!m_running) {
        m_running = true;
        m_remainingTime = m_workTime * 60;
        m_workTimer.start(m_remainingTime * 1000);
        m_countdownTimer.start();
        emit sessionStart();
    }
}

void TimerManager::pause()
{
    if (m_running) {
        m_workTimer.stop();
        m_countdownTimer.stop();
        m_running = false;
        emit sessionEnd();
    }
}

void TimerManager::resume()
{
    if (!m_running) {
        m_running = true;
        m_workTimer.start(m_remainingTime * 1000);
        m_countdownTimer.start();
        emit sessionStart();
    }
}

void TimerManager::stop()
{
    if (m_running) {
        m_workTimer.stop();
        m_countdownTimer.stop();
        m_running = false;
        emit sessionEnd();
    }
}

void TimerManager::reset()
{
    stop();
    m_remainingTime = m_workTime * 60;
}

int TimerManager::getRemainingTime() const
{
    return m_remainingTime;
}

bool TimerManager::isRunning() const
{
    return m_running;
}

void TimerManager::onTimerTick()
{
    if (m_remainingTime > 0) {
        m_remainingTime--;
    }
}

void TimerManager::onWorkTimerTimeout()
{
    m_countdownTimer.stop();
    m_running = false;
    emit sessionEnd();
    emit timerComplete();
}
