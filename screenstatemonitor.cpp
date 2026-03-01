// 确保Windows头文件在Qt之前包含
#if defined(_WIN32) || defined(Q_OS_WIN)
#ifndef Q_OS_WIN
#define Q_OS_WIN
#endif
#include <windows.h>
#include <wtsapi32.h>

// Define WM_ACTIVATE constants if not available
#ifndef WA_ACTIVE
#define WA_ACTIVE 1
#define WA_CLICKACTIVE 2
#endif

// Compatibility definitions for MinGW
// Some MinGW versions don't define WTS_CONNECTSTATE_CLASS properly
#ifndef WTS_CONNECTSTATE_CLASS
#define WTS_CONNECTSTATE_CLASS static_cast<WTS_INFO_CLASS>(8)
#endif
#endif

#include "ScreenStateMonitor.h"
#include <QCoreApplication>

#if defined(Q_OS_LINUX)
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>

class LinuxDBusPrivate : public QObject
{
    Q_OBJECT
public:
    LinuxDBusPrivate(ScreenStateMonitor *parent) : m_parent(parent) {
        // 1. 监听GNOME屏保（org.gnome.ScreenSaver）
        QDBusConnection bus = QDBusConnection::sessionBus();
        bus.connect(
            "org.gnome.ScreenSaver", "/org/gnome/ScreenSaver",
            "org.gnome.ScreenSaver", "ActiveChanged",
            this, SLOT(onGnomeScreenSaverActiveChanged(bool))
            );

        // 2. 监听GNOME锁屏（org.freedesktop.login1）
        bus.connect(
            "org.freedesktop.login1", "/org/freedesktop/login1/session/self",
            "org.freedesktop.login1.Session", "LockHint",
            this, SLOT(onGnomeLockHintChanged(bool))
            );

        // 3. 监听KDE屏保（org.kde.screensaver）
        bus.connect(
            "org.kde.screensaver", "/ScreenSaver",
            "org.freedesktop.ScreenSaver", "ActiveChanged",
            this, SLOT(onKdeScreenSaverActiveChanged(bool))
            );
    }

private slots:
    void onGnomeScreenSaverActiveChanged(bool active) {
        emit m_parent->screenStateChanged(active ? ScreenState::ScreenSaverStart : ScreenState::ScreenSaverStop);
    }

    void onGnomeLockHintChanged(bool locked) {
        emit m_parent->screenStateChanged(locked ? ScreenState::Locked : ScreenState::UnlockedFromLock);
    }

    void onKdeScreenSaverActiveChanged(bool active) {
        emit m_parent->screenStateChanged(active ? ScreenState::ScreenSaverStart : ScreenState::ScreenSaverStop);
    }

private:
    ScreenStateMonitor *m_parent;
};
#endif

// ===================== 跨平台入口 =====================
ScreenStateMonitor::ScreenStateMonitor(QObject *parent)
    : QObject(parent)
{
    initPlatformMonitor();
}

ScreenStateMonitor::~ScreenStateMonitor()
{
    destroyPlatformMonitor();
}

void ScreenStateMonitor::initPlatformMonitor()
{
#if defined(Q_OS_WIN)
    // ===================== Windows实现 =====================
    // 创建隐藏窗口用于接收系统消息
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = L"ScreenStateMonitorClass";

    if (!RegisterClassEx(&wc)) {
        qWarning("Windows: 注册窗口类失败");
        return;
    }

    m_hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"ScreenStateMonitorWindow",
        0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, this
        );

    if (!m_hwnd) {
        qWarning("Windows: 创建隐藏窗口失败");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // 注册接收会话变化消息（锁屏/解锁）
    WTSRegisterSessionNotification(m_hwnd, NOTIFY_FOR_THIS_SESSION);

#elif defined(Q_OS_LINUX)
    // ===================== Linux实现（DBus） =====================
    m_dbusPrivate = new LinuxDBusPrivate(this);

#elif defined(Q_OS_MACOS)
// ===================== macOS实现（Objective-C） =====================
#include <objc/objc.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>

    // 封装Cocoa的通知监听
    static void screenStateCallback(id observer, SEL, id note) {
        QString noteName = QString::fromNSString((NSString*)objc_msgSend(note, sel_registerName("name")));
        ScreenStateMonitor *monitor = (ScreenStateMonitor*)objc_getAssociatedObject(observer, "monitor");
        if (!monitor) return;

        if (noteName == @"com.apple.screensaver.didstart") {
            emit monitor->screenStateChanged(ScreenState::ScreenSaverStart);
        } else if (noteName == @"com.apple.screensaver.didstop") {
            emit monitor->screenStateChanged(ScreenState::ScreenSaverStop);
        } else if (noteName == @"com.apple.screenIsLocked") {
            emit monitor->screenStateChanged(ScreenState::Locked);
        } else if (noteName == @"com.apple.screenIsUnlocked") {
            emit monitor->screenStateChanged(ScreenState::UnlockedFromLock);
        }
    }

    // 初始化Cocoa通知
    void *cls = objc_getClass("NSObject");
    m_observer = objc_msgSend(cls, sel_registerName("new"));
    objc_setAssociatedObject(m_observer, "monitor", this, OBJC_ASSOCIATION_RETAIN_NONATOMIC);

    // 注册通知
    id nc = objc_msgSend(objc_getClass("NSNotificationCenter"), sel_registerName("defaultCenter"));
    SEL addObserver = sel_registerName("addObserver:selector:name:object:");
    SEL callbackSel = sel_registerName("screenStateCallback:");

    // 注册屏保启动/停止通知
    objc_msgSend(nc, addObserver, m_observer, callbackSel,
                 objc_msgSend(objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "com.apple.screensaver.didstart"), nullptr);
    objc_msgSend(nc, addObserver, m_observer, callbackSel,
                 objc_msgSend(objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "com.apple.screensaver.didstop"), nullptr);

    // 注册锁屏/解锁通知
    objc_msgSend(nc, addObserver, m_observer, callbackSel,
                 objc_msgSend(objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "com.apple.screenIsLocked"), nullptr);
    objc_msgSend(nc, addObserver, m_observer, callbackSel,
                 objc_msgSend(objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "com.apple.screenIsUnlocked"), nullptr);

    // 注册回调方法
    class_addMethod((Class)cls, callbackSel, (IMP)screenStateCallback, "v@:@");
#endif
}

void ScreenStateMonitor::destroyPlatformMonitor()
{
#if defined(Q_OS_WIN)
    if (m_hwnd) {
        WTSUnRegisterSessionNotification(m_hwnd);
        DestroyWindow(m_hwnd);
        UnregisterClass(L"ScreenStateMonitorClass", GetModuleHandle(nullptr));
    }
#elif defined(Q_OS_LINUX)
    delete m_dbusPrivate;
#elif defined(Q_OS_MACOS)
    if (m_observer) {
        id nc = objc_msgSend(objc_getClass("NSNotificationCenter"), sel_registerName("defaultCenter"));
        objc_msgSend(nc, sel_registerName("removeObserver:"), m_observer);
        objc_msgSend(m_observer, sel_registerName("release"));
    }
#endif
}

#if defined(Q_OS_WIN)
// Windows窗口过程函数（处理系统消息）
LRESULT CALLBACK ScreenStateMonitor::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE) {
        // 将当前类指针关联到窗口
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return 0;
    }

    ScreenStateMonitor *monitor = (ScreenStateMonitor*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!monitor) return DefWindowProc(hwnd, msg, wParam, lParam);

    switch (msg) {
    case WM_SYSCOMMAND: // 屏保相关消息
        if (wParam == SC_SCREENSAVE) { // 屏保启动
            emit monitor->screenStateChanged(ScreenState::ScreenSaverStart);
        }
        break;

    case WM_WTSSESSION_CHANGE: // 锁屏/解锁相关消息
        switch (wParam) {
        case WTS_SESSION_LOCK: // 锁屏
            emit monitor->screenStateChanged(ScreenState::Locked);
            break;
        case WTS_SESSION_UNLOCK: // 解锁
            emit monitor->screenStateChanged(ScreenState::UnlockedFromLock);
            break;
        }
        break;
        
    case WM_ACTIVATE: // 应用程序激活/取消激活（用于检测屏保退出）
        // When wParam is WA_ACTIVE or WA_CLICKACTIVE, it means our app is being activated,
        // which often happens when screensaver stops
        if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) {
            // 应用程序被激活，可能是屏保退出
            emit monitor->screenStateChanged(ScreenState::ScreenSaverStop);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
#endif

bool ScreenStateMonitor::isUserLoggedIn() const
{
#if defined(Q_OS_WIN)
    // Windows: Check if the session is locked
    // Method 1: Check if there's a foreground window (simple but not 100% reliable)
    HWND foregroundWnd = GetForegroundWindow();
    if (!foregroundWnd) {
        // No foreground window, likely in lock screen
        return false;
    }
    
    // Method 2: Use WTS API to check session state (more reliable and compatible)
    // WTS_CURRENT_SERVER_HANDLE might be defined as DWORD in some SDK versions
    // Use explicit casting to HANDLE to avoid conversion errors
    HANDLE hServer = reinterpret_cast<HANDLE>(WTS_CURRENT_SERVER_HANDLE);
    
    // WTS_CURRENT_SESSION is a DWORD (typically (DWORD)-1), not a HANDLE
    DWORD sessionId = WTS_CURRENT_SESSION;
    
    // Query the connection state using the ConnectState info class
    LPWSTR pBuffer = NULL;
    DWORD bytesReturned = 0;
    
    // Use WTS_CONNECTSTATE_CLASS (enum value) to query connection state
    // This is supported on all Windows versions
    BOOL result = WTSQuerySessionInformation(hServer, sessionId, WTS_CONNECTSTATE_CLASS, &pBuffer, &bytesReturned);
    if (result && pBuffer && bytesReturned >= sizeof(DWORD)) {
        // The returned data is a DWORD representing the connection state
        DWORD connectStateValue = *reinterpret_cast<DWORD*>(pBuffer);
        WTSFreeMemory(pBuffer);
        
        // Check if session is active based on the state value
        // State 0 (WTSActive) means user is logged in and active
        if (connectStateValue == 0 /* WTSActive */) {
            return true;  // User is actively logged in
        } else if (connectStateValue == 4 /* WTSDisconnected */ || connectStateValue == 8 /* WTSDown */) {
            return false; // Session is disconnected
        }
        // For other states (1-3, 5-7, 9), continue to fallback check
    }
    
    // Fallback: Check if we can get user name - if not, likely locked
    pBuffer = NULL;
    bytesReturned = 0;
    result = WTSQuerySessionInformation(hServer, sessionId, WTSUserName, &pBuffer, &bytesReturned);
    if (result && pBuffer && bytesReturned > 0) {
        QString userName = QString::fromWCharArray(pBuffer);
        WTSFreeMemory(pBuffer);
        
        // If we can get a username, user is likely logged in
        return !userName.isEmpty();
    }
    
    // If we can't get user name, assume session is locked
    if (pBuffer) {
        WTSFreeMemory(pBuffer);
    }
    
    return false;
    
#elif defined(Q_OS_LINUX)
    // Linux: Check if screen is locked via DBus
    // This is a simplified check - in production, you'd want more robust detection
    QDBusConnection bus = QDBusConnection::sessionBus();
    
    // Check GNOME Screensaver
    QDBusInterface gnomeScreenSaver("org.gnome.ScreenSaver", "/org/gnome/ScreenSaver",
                                     "org.gnome.ScreenSaver", bus);
    if (gnomeScreenSaver.isValid()) {
        QDBusReply<bool> reply = gnomeScreenSaver.call("GetActive");
        if (reply.isValid()) {
            return !reply.value(); // If screensaver is active, user is not logged in
        }
    }
    
    // Check KDE Screensaver
    QDBusInterface kdeScreenSaver("org.kde.screensaver", "/ScreenSaver",
                                   "org.freedesktop.ScreenSaver", bus);
    if (kdeScreenSaver.isValid()) {
        QDBusReply<bool> reply = kdeScreenSaver.call("GetActive");
        if (reply.isValid()) {
            return !reply.value(); // If screensaver is active, user is not logged in
        }
    }
    
    // If we can't determine, assume user is logged in
    return true;
    
#elif defined(Q_OS_MACOS)
    // macOS: Check if screen is locked
    // This requires CoreGraphics framework
    
    // Use proper function pointer casting for objc_msgSend
    typedef id (*OBJC_MSG_SEND_ID)(id, SEL);
    typedef id (*OBJC_MSG_SEND_ID_SEL_ID)(id, SEL, id);
    typedef BOOL (*OBJC_MSG_SEND_BOOL)(id, SEL);
    
    OBJC_MSG_SEND_ID msgSend_id = (OBJC_MSG_SEND_ID)objc_msgSend;
    OBJC_MSG_SEND_ID_SEL_ID msgSend_id_sel_id = (OBJC_MSG_SEND_ID_SEL_ID)objc_msgSend;
    OBJC_MSG_SEND_BOOL msgSend_bool = (OBJC_MSG_SEND_BOOL)objc_msgSend;
    
    id sessionDict = msgSend_id((id)objc_getClass("CGSSession"), 
                                 sel_registerName("copyCurrentSessionDictionary"));
    if (sessionDict) {
        // Check if screen is locked
        id key = msgSend_id((id)objc_getClass("NSString"), 
                             sel_registerName("stringWithUTF8String:"), 
                             "CGSSessionScreenIsLocked");
        id lockedValue = msgSend_id_sel_id(sessionDict, 
                                            sel_registerName("objectForKey:"), 
                                            key);
        if (lockedValue) {
            BOOL isLocked = msgSend_bool(lockedValue, sel_registerName("boolValue"));
            return !isLocked; // Return true if NOT locked
        }
    }
    
    // Default to true if we can't determine
    return true;
    
#else
    // Other platforms: default to true
    return true;
#endif
}

#if defined(Q_OS_LINUX)
#include "screenstatemonitor.moc"
#endif
