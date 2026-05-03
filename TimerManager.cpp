#include "ConfigManager.h"
#include "TimerManager.h"
#include <windows.h>

TimerManager::TimerManager(QObject *parent)
    : QObject(parent)
    , m_workTime(20), m_breakTime(20), m_remainingTime(20 * 60)
    , m_running(false), m_saverRunning(false)
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &TimerManager::onTimerTick);
    m_timer.start();
}

TimerManager::~TimerManager()
{
    m_timer.stop();
}

void TimerManager::setWorkTime(int minutes)
{
    m_workTime = minutes;
    m_remainingTime = minutes * 60;
    if (m_remainingTime) {

    }
}

void TimerManager::setBreakTime(int seconds)
{
    m_breakTime = seconds;
}

void TimerManager::start()
{
    m_running = true;
    m_remainingTime = m_workTime * 60;
    emit workStart();
}

void TimerManager::pause()
{
    m_running = false;
    emit workEnd();
}

void TimerManager::resume()
{
    m_running = true;
    emit workStart();
}

void TimerManager::stop()
{
    m_running = false;
    emit workEnd();
}

void TimerManager::reset()
{
    m_remainingTime = m_workTime * 60;
}

void TimerManager::setAutoFlag(bool flag)
{
    m_autoFlag = flag;
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
    if (m_autoFlag && m_running && !m_saverRunning && m_remainingTime > 0) {
        m_remainingTime--;
    }
    if (m_remainingTime == 0) {
        m_running = false;
        m_remainingTime = -1;
        emit workEnd();
        emit lookAwayTrigger();
    }
#if defined(Q_OS_WIN)
    auto saverFlag = ConfigManager::instance()->screenSaverOption();
    if(saverFlag == 0){
        m_saverRunning = false;
    }else{
        bool saverRunning{false};
        ::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &saverRunning, 0);
        m_saverRunning = saverRunning;
    }
#endif
}
