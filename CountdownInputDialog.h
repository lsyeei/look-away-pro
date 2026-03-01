#ifndef COUNTDOWNINPUTDIALOG_H
#define COUNTDOWNINPUTDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>

class CountdownInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CountdownInputDialog(QWidget *parent = nullptr);
    ~CountdownInputDialog();
    
    int hours() const;
    int minutes() const;
    int seconds() const;

private:
    void setupUI();
    
    QSpinBox *m_hourSpinBox;
    QSpinBox *m_minuteSpinBox;
    QSpinBox *m_secondSpinBox;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // COUNTDOWNINPUTDIALOG_H
