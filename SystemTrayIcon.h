#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    void setPaused(bool paused);

signals:
    void showStatistics();
    void showSettings();
    void pauseResume();
    void showAbout();
    void quit();
    void showTimer();
    void showCountdown();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createMenu();

    QMenu *m_menu;
    QMenu *m_toolsMenu;
    QAction *m_statisticsAction;
    QAction *m_settingsAction;
    QAction *m_pauseAction;
    QAction *m_aboutAction;
    QAction *m_quitAction;
    QAction *m_timerAction;
    QAction *m_countdownAction;
};

#endif // SYSTEMTRAYICON_H
