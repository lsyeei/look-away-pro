#ifndef COUNTDOWNINPUTDIALOG_H
#define COUNTDOWNINPUTDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

class CountdownInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CountdownInputDialog(QWidget *parent = nullptr);
    ~CountdownInputDialog();
    
    int hours() const;
    int minutes() const;
    int seconds() const;
    int endAction() const;
    QString killProcess() const;

private slots:
    void onEndActionChanged(int index);

private:
    void setupUI();
    
    QSpinBox *m_hourSpinBox;
    QSpinBox *m_minuteSpinBox;
    QSpinBox *m_secondSpinBox;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QComboBox *m_endActionComboBox;
    QLabel *m_processLabel;
    QLineEdit *m_processEdit;
};

#endif // COUNTDOWNINPUTDIALOG_H
