#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();

    QVBoxLayout *m_mainLayout;
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QLabel *m_versionLabel;
    QLabel *m_authorLabel;
    QLabel *m_emailLabel;
    QLabel *m_projectLabel;
    QPushButton *m_closeButton;
};

#endif // ABOUTDIALOG_H
