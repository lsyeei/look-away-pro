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
    , m_endActionComboBox(new QComboBox(this))
    , m_processEdit(new QLineEdit(this))
{
    setupUI();
}

CountdownInputDialog::~CountdownInputDialog()
{
}

void CountdownInputDialog::setupUI()
{
    setWindowTitle("设置倒计时");
    
    m_hourSpinBox->setRange(0, 23);
    m_hourSpinBox->setSuffix(" 时");
    m_hourSpinBox->setValue(0);
    m_hourSpinBox->setAlignment(Qt::AlignRight);
    m_hourSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    
    m_minuteSpinBox->setRange(0, 59);
    m_minuteSpinBox->setSuffix(" 分");
    m_minuteSpinBox->setValue(5);
    m_minuteSpinBox->setAlignment(Qt::AlignRight);
    m_minuteSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    
    m_secondSpinBox->setRange(0, 59);
    m_secondSpinBox->setSuffix(" 秒");
    m_secondSpinBox->setValue(0);
    m_secondSpinBox->setAlignment(Qt::AlignRight);
    m_secondSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    
    auto *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(m_hourSpinBox);
    timeLayout->addWidget(new QLabel(":"));
    timeLayout->addWidget(m_minuteSpinBox);
    timeLayout->addWidget(new QLabel(":"));
    timeLayout->addWidget(m_secondSpinBox);
    timeLayout->setSpacing(0);

    // 结束动作
    m_endActionComboBox->addItem("闪烁");
    m_endActionComboBox->addItem("锁屏");
    m_endActionComboBox->addItem("关闭进程");

    m_processLabel = new QLabel("进程名");
    m_processEdit->setPlaceholderText("如 notepad.exe");
    m_processEdit->setClearButtonEnabled(true);
    m_processEdit->setVisible(false);

    auto *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(new QLabel("计时结束"));
    actionLayout->addWidget(m_endActionComboBox);

    auto *processLayout = new QHBoxLayout();
    processLayout->addWidget(m_processLabel);
    processLayout->addWidget(m_processEdit);
    processLayout->setContentsMargins(0, 0, 0, 0);

    connect(m_endActionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CountdownInputDialog::onEndActionChanged);
    
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(timeLayout);
    mainLayout->addLayout(actionLayout);
    mainLayout->addLayout(processLayout);
    mainLayout->addWidget(buttonBox);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    setLayout(mainLayout);
}

void CountdownInputDialog::onEndActionChanged(int index)
{
    m_processLabel->setVisible(index == 2);
    m_processEdit->setVisible(index == 2);
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

int CountdownInputDialog::endAction() const
{
    return m_endActionComboBox->currentIndex();
}

QString CountdownInputDialog::killProcess() const
{
    return m_processEdit->text().trimmed();
}
