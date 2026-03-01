#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QDateEdit>
#include "DatabaseManager.h"
#include <QPushButton>

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~StatisticsDialog();

    void updateStatistics();

private:
    Ui::StatisticsDialog *ui;
    DatabaseManager *m_databaseManager;

    void loadTodayStatistics();
    void loadWeekStatistics();
    void loadMonthStatistics();
    void loadCustomStatistics();
    void formatTime(qint64 seconds, QString &hours, QString &minutes, QString &total);
};

#endif // STATISTICSDIALOG_H
