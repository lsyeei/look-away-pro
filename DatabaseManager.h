#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool open();
    void close();

    void startWorkSession();
    void endWorkSession();
    void recordBreakTime(qint64 duration);

    qint64 getTotalWorkTime(const QDateTime &start, const QDateTime &end);
    qint64 getTotalBreakTime(const QDateTime &start, const QDateTime &end);

private:
    QSqlDatabase m_database;
    qint64 m_currentSessionStartTime;

    bool initializeDatabase();
};

#endif // DATABASEMANAGER_H
