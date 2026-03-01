#include "CountdownInputDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

CountdownInputDialog::CountdownInputDialog(QWidget *parent)
    : QDialog(parent)
    , m_hourSpinBox(new QSpinBox(this))
    , m_minuteSpinBox(new QSpinBox(this))
    , m_secondSpinBox(new QSpinBox(this))
{
    setupUI();
}

CountdownInputDialog::~CountdownInputDialog()
{
}

void CountdownInputDialog::setupUI()
{
    setWindowTitle("设置倒计时");
    setFixedSize(300, 150);
    
    m_hourSpinBox->setRange(0, 23);
    m_hourSpinBox->setSuffix(" 时");
    m_hourSpinBox->setValue(0);
    
    m_minuteSpinBox->setRange(0, 59);
    m_minuteSpinBox->setSuffix(" 分");
    m_minuteSpinBox->setValue(5);
    
    m_secondSpinBox->setRange(0, 59);
    m_secondSpinBox->setSuffix(" 秒");
    m_secondSpinBox->setValue(0);
    
    auto *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(m_hourSpinBox);
    timeLayout->addWidget(m_minuteSpinBox);
    timeLayout->addWidget(m_secondSpinBox);
    timeLayout->setSpacing(10);
    
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(timeLayout);
    mainLayout->addWidget(buttonBox);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    setLayout(mainLayout);
}

int CountdownInputDialog::hours() const
{
    return m_hourSpinBox->value();
}

int CountdownInputDialog::minutes() const
{
    return m_minuteSpinBox->value();
}

int CountdownInputDialog::seconds() const
{
    return m_secondSpinBox->value();
}
