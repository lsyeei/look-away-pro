#include "DatabaseManager.h"
#include <QSqlError>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_currentSessionStartTime(0)
{
    if (!initializeDatabase()) {
        qCritical("数据库初始化失败");
        return;
    }
    if (!open()) {
        qCritical("数据库打开失败");
    }
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::initializeDatabase()
{
    // Use application directory for database storage
    QString appPath = QCoreApplication::applicationDirPath();
    QString dbPath = appPath + "/LookAwayPro.db";
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    return true;
}

bool DatabaseManager::open()
{
    if (!m_database.open()) {
        qDebug() << "Database open error:" << m_database.lastError().text();
        return false;
    }

    QSqlQuery query;
    QString createWorkSessionsTable = R"(
        CREATE TABLE IF NOT EXISTS work_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            start_time INTEGER NOT NULL,
            end_time INTEGER NOT NULL,
            duration INTEGER NOT NULL
        )
    )";

    if (!query.exec(createWorkSessionsTable)) {
        qDebug() << "Create work_sessions table error:" << query.lastError().text();
        return false;
    }

    QString createBreakSessionsTable = R"(
        CREATE TABLE IF NOT EXISTS break_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            start_time INTEGER NOT NULL,
            duration INTEGER NOT NULL
        )
    )";

    if (!query.exec(createBreakSessionsTable)) {
        qDebug() << "Create break_sessions table error:" << query.lastError().text();
        return false;
    }

    return true;
}

void DatabaseManager::close()
{
    if (m_currentSessionStartTime > 0) {
        endWorkSession();
    }
    m_database.close();
}

void DatabaseManager::startWorkSession()
{
    m_currentSessionStartTime = QDateTime::currentDateTime().toSecsSinceEpoch();
}

void DatabaseManager::endWorkSession()
{
    if (m_currentSessionStartTime <= 0) {
        return;
    }

    qint64 endTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    qint64 duration = endTime - m_currentSessionStartTime;

    QSqlQuery query;
    query.prepare("INSERT INTO work_sessions (start_time, end_time, duration) VALUES (?, ?, ?)");
    query.addBindValue(m_currentSessionStartTime);
    query.addBindValue(endTime);
    query.addBindValue(duration);

    if (!query.exec()) {
        qDebug() << "Insert work session error:" << query.lastError().text();
    }

    m_currentSessionStartTime = 0;
}

void DatabaseManager::recordBreakTime(qint64 duration)
{
    qint64 startTime = QDateTime::currentDateTime().toSecsSinceEpoch();

    QSqlQuery query;
    query.prepare("INSERT INTO break_sessions (start_time, duration) VALUES (?, ?)");
    query.addBindValue(startTime);
    query.addBindValue(duration);

    if (!query.exec()) {
        qDebug() << "Insert break session error:" << query.lastError().text();
    }
}

qint64 DatabaseManager::getTotalWorkTime(const QDateTime &start, const QDateTime &end)
{
    qint64 startTime = start.toSecsSinceEpoch();
    qint64 endTime = end.toSecsSinceEpoch();

    QSqlQuery query;
    query.prepare("SELECT SUM(duration) FROM work_sessions WHERE start_time >= ? AND end_time <= ?");
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (!query.exec()) {
        qDebug() << "Get work time error:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toLongLong();
    }

    return 0;
}

qint64 DatabaseManager::getTotalBreakTime(const QDateTime &start, const QDateTime &end)
{
    qint64 startTime = start.toSecsSinceEpoch();
    qint64 endTime = end.toSecsSinceEpoch();

    QSqlQuery query;
    query.prepare("SELECT SUM(duration) FROM break_sessions WHERE start_time >= ? AND start_time <= ?");
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (!query.exec()) {
        qDebug() << "Get break time error:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toLongLong();
    }

    return 0;
}
