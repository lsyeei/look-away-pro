#include "ConfigManager.h"
#include "qdebug.h"
#include "TimerManager.h"
#include <windows.h>

TimerManager::TimerManager(QObject *parent)
    : QObject(parent)
    , m_workTime(20), m_breakTime(20), m_remainingTime(20 * 60)
    , m_running(true), m_autoFlag(true)
    , m_saverRunning(false), m_screenLocked(false)
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
    reset();
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

void TimerManager::setScreenLocked(bool flag)
{
    auto option = ConfigManager::instance()->screenSaverOption();
    if (option == 0) {
        m_screenLocked = false;
        return;
    }
    m_screenLocked = flag;
    if (option == 2) {
        reset();
    }
}

int TimerManager::getRemainingTime() const
{
    return m_remainingTime;
}

void TimerManager::onTimerTick()
{
    bool runState =  m_autoFlag && !m_screenLocked && !m_saverRunning && (m_remainingTime > 0);
    if (runState != m_running) {
        if (runState) {
            emit workStart();
        }else{
            emit workEnd();
            if (m_autoFlag && m_remainingTime <= 0) {
                emit lookAwayTrigger();
            }
        }
    }
    m_running = runState;
    if (m_running) {
        m_remainingTime--;
    }
#if defined(Q_OS_WIN)
    auto saverFlag = ConfigManager::instance()->screenSaverOption();
    if(saverFlag == 0){
        m_saverRunning = false;
    }else{
        bool saverRunning{false};
        ::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &saverRunning, 0);
        if (saverFlag == 2 && saverRunning && !m_saverRunning) {
            reset();
        }
        m_saverRunning = saverRunning;
    }
#endif
}
