#include "StatisticsDialog.h"
#include "ui_StatisticsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDate>
#include <QDateTime>
#include <QPushButton>

StatisticsDialog::StatisticsDialog(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , m_databaseManager(dbManager)
{
    ui->setupUi(this);

    // Setup tab widget connections
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 0) {
            loadTodayStatistics();
        } else if (index == 1) {
            loadWeekStatistics();
        } else if (index == 2) {
            loadMonthStatistics();
        } else if (index == 3) {
            loadCustomStatistics();
        }
    });

    connect(ui->startDateEdit, &QDateEdit::dateChanged, this, [this]() {
        if (ui->tabWidget->currentIndex() == 3) {
            loadCustomStatistics();
        }
    });
    connect(ui->endDateEdit, &QDateEdit::dateChanged, this, [this]() {
        if (ui->tabWidget->currentIndex() == 3) {
            loadCustomStatistics();
        }
    });

    // Set default date range for custom tab (one year)
    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addYears(-1);
    ui->startDateEdit->setDate(startDate);
    ui->endDateEdit->setDate(endDate);

    // Load today's statistics by default
    loadTodayStatistics();
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::updateStatistics()
{
    if (ui->tabWidget->currentIndex() == 0) {
        loadTodayStatistics();
    } else if (ui->tabWidget->currentIndex() == 1) {
        loadWeekStatistics();
    } else if (ui->tabWidget->currentIndex() == 2) {
        loadMonthStatistics();
    } else {
        loadCustomStatistics();
    }
}

void StatisticsDialog::loadTodayStatistics()
{
    QDate today = QDate::currentDate();
    QDateTime start(today.startOfDay());
    QDateTime end(today.endOfDay());

    qint64 workTime = m_databaseManager->getTotalWorkTime(start, end);
    qint64 breakTime = m_databaseManager->getTotalBreakTime(start, end);

    QString workHours, workMinutes, workTotal;
    QString breakHours, breakMinutes, breakTotal;

    formatTime(workTime, workHours, workMinutes, workTotal);
    formatTime(breakTime, breakHours, breakMinutes, breakTotal);

    ui->workHoursLabel->setText(workHours);
    ui->workMinutesLabel->setText(workMinutes);

    ui->breakHoursLabel->setText(breakHours);
    ui->breakMinutesLabel->setText(breakMinutes);
}

void StatisticsDialog::loadWeekStatistics()
{
    QDate today = QDate::currentDate();
    QDate startOfWeek = today.addDays(-(today.dayOfWeek() - 1));
    QDateTime start(startOfWeek.startOfDay());
    QDateTime end(today.endOfDay());

    qint64 workTime = m_databaseManager->getTotalWorkTime(start, end);
    qint64 breakTime = m_databaseManager->getTotalBreakTime(start, end);

    QString workHours, workMinutes, workTotal;
    QString breakHours, breakMinutes, breakTotal;

    formatTime(workTime, workHours, workMinutes, workTotal);
    formatTime(breakTime, breakHours, breakMinutes, breakTotal);

    ui->workHoursLabel_2->setText(workHours);
    ui->workMinutesLabel_2->setText(workMinutes);

    ui->breakHoursLabel_2->setText(breakHours);
    ui->breakMinutesLabel_2->setText(breakMinutes);
}

void StatisticsDialog::loadMonthStatistics()
{
    QDate today = QDate::currentDate();
    QDate startOfMonth = today.addDays(-(today.day() - 1));
    QDateTime start(startOfMonth.startOfDay());
    QDateTime end(today.endOfDay());

    qint64 workTime = m_databaseManager->getTotalWorkTime(start, end);
    qint64 breakTime = m_databaseManager->getTotalBreakTime(start, end);

    QString workHours, workMinutes, workTotal;
    QString breakHours, breakMinutes, breakTotal;

    formatTime(workTime, workHours, workMinutes, workTotal);
    formatTime(breakTime, breakHours, breakMinutes, breakTotal);

    ui->workHoursLabel_3->setText(workHours);
    ui->workMinutesLabel_3->setText(workMinutes);

    ui->breakHoursLabel_3->setText(breakHours);
    ui->breakMinutesLabel_3->setText(breakMinutes);
}

void StatisticsDialog::loadCustomStatistics()
{
    QDateTime start(ui->startDateEdit->date().startOfDay());
    QDateTime end(ui->endDateEdit->date().endOfDay());

    qint64 workTime = m_databaseManager->getTotalWorkTime(start, end);
    qint64 breakTime = m_databaseManager->getTotalBreakTime(start, end);

    QString workHours, workMinutes, workTotal;
    QString breakHours, breakMinutes, breakTotal;

    formatTime(workTime, workHours, workMinutes, workTotal);
    formatTime(breakTime, breakHours, breakMinutes, breakTotal);

    ui->workHoursLabel_4->setText(workHours);
    ui->workMinutesLabel_4->setText(workMinutes);

    ui->breakHoursLabel_4->setText(breakHours);
    ui->breakMinutesLabel_4->setText(breakMinutes);
}

void StatisticsDialog::formatTime(qint64 seconds, QString &hours, QString &minutes, QString &total)
{
    qint64 h = seconds / 3600;
    qint64 m = (seconds % 3600) / 60;
    qint64 s = seconds % 60;

    hours = QString::number(h);
    minutes = QString::number(m);
    total = QString("%1小时 %2分钟 %3秒").arg(h).arg(m).arg(s);
}
