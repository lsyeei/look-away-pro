#ifndef SCREENSTATEMONITOR_H
#define SCREENSTATEMONITOR_H

// 确保Windows头文件在Qt之前包含
#if defined(_WIN32) || defined(Q_OS_WIN)
#ifndef Q_OS_WIN
#define Q_OS_WIN
#endif
#include <windows.h>
#include <wtsapi32.h>

// 定义WM_ACTIVATE的wParam值（如果没有定义）
#ifndef WA_ACTIVE
#define WA_ACTIVE 1
#endif
#ifndef WA_CLICKACTIVE
#define WA_CLICKACTIVE 2
#endif
#endif

#include <QObject>
#include <QString>

// 屏幕状态枚举
enum class ScreenState {
    Unlocked,        // 未锁屏/屏保未启动
    ScreenSaverStart, // 屏保启动
    ScreenSaverStop,  // 屏保退出
    Locked,           // 系统锁屏
    UnlockedFromLock  // 从系统锁屏解锁
};

// 前向声明Linux私有类
#if defined(Q_OS_LINUX)
class LinuxDBusPrivate;
#endif

class ScreenStateMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ScreenStateMonitor(QObject *parent = nullptr);
    ~ScreenStateMonitor() override;

signals:
    // 屏幕状态变化信号（对外暴露）
    void screenStateChanged(ScreenState state);

public:
    // 检查当前用户是否已登录系统（未在锁屏界面）
    // 主要用于屏保退出后判断是否需要密码解锁
    bool isUserLoggedIn() const;

private:
    // 平台相关初始化/销毁
    void initPlatformMonitor();
    void destroyPlatformMonitor();

#if defined(Q_OS_WIN)
    // Windows：窗口过程函数（处理系统消息）
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND m_hwnd = nullptr; // 隐藏窗口句柄（用于接收系统消息）
#elif defined(Q_OS_LINUX)
    // Linux：DBus相关
    LinuxDBusPrivate *m_dbusPrivate = nullptr;
#elif defined(Q_OS_MACOS)
    // macOS：Objective-C相关
    void *m_observer = nullptr; // 通知观察者
#endif
};

#endif // SCREENSTATEMONITOR_H
