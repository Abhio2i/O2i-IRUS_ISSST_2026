#include "timetoolbar.h"
#include <QToolButton>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>

TimeToolBar::TimeToolBar(QWidget* parent) : QToolBar(tr("Time"), parent)
{
    setObjectName(QStringLiteral("TimeToolbar"));
    setMovable(false);
    setFloatable(false);

    m_button = new QToolButton(this);
    m_button->setText("00:00:00");
    m_button->setToolTip(tr("Click to set time"));
    m_button->setCursor(Qt::PointingHandCursor);
    m_button->setStyleSheet(
        "QToolButton { color: white; font-weight: bold; font-size: 12px; "
        "background-color: #1A3652;"
        "border-radius: 3px; padding: 2px 8px; }"
        "QToolButton:hover { background-color: #27446d; }");
    addWidget(m_button);
    connect(m_button, &QToolButton::clicked, this, &TimeToolBar::onButtonClicked);
}

void TimeToolBar::setTime(float seconds)
{
    m_seconds = seconds;
    int s = static_cast<int>(seconds);
    m_button->setText(QString("%1:%2:%3")
                          .arg(s / 3600, 2, 10, QChar('0'))
                          .arg((s % 3600) / 60, 2, 10, QChar('0'))
                          .arg(s % 60, 2, 10, QChar('0')));
}

void TimeToolBar::onButtonClicked()
{
    int total = static_cast<int>(m_seconds);

    QDialog dialog(this);
    dialog.setWindowTitle("Set Time");
    dialog.setModal(true);
    dialog.setStyleSheet(
        "QDialog { background-color: #0F2636; color: white; }"
        "QLabel { color: white; }"
        "QLineEdit { background-color: #1A3652; color: white;  }"
        "QPushButton { background-color: #1A3652; color: white; border: 1px solid #0078D4; padding: 4px 12px; }");

    auto makeEdit = [&dialog](const QString& ph, int val, int maxLen, int maxVal) {
        auto* e = new QLineEdit(&dialog);
        e->setPlaceholderText(ph);
        e->setText(QString::number(val));
        e->setMaxLength(maxLen);
        e->setFixedWidth(50);
        e->setValidator(new QIntValidator(0, maxVal, e));
        return e;
    };
    QLineEdit* hh = makeEdit("HH", total / 3600,        3, 999);
    QLineEdit* mm = makeEdit("MM", (total % 3600) / 60, 2, 59);
    QLineEdit* ss = makeEdit("SS", total % 60,          2, 59);

    QHBoxLayout* timeLayout = new QHBoxLayout();
    timeLayout->addWidget(new QLabel("Time:", &dialog));
    timeLayout->addWidget(hh);
    timeLayout->addWidget(mm);
    timeLayout->addWidget(ss);

    QPushButton* ok = new QPushButton("OK", &dialog);
    QPushButton* cancel = new QPushButton("Cancel", &dialog);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(ok);
    btnLayout->addWidget(cancel);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addLayout(timeLayout);
    layout->addLayout(btnLayout);

    connect(ok, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        float t = hh->text().toInt() * 3600
                  + mm->text().toInt() * 60
                  + ss->text().toInt();
        setTime(t);
        emit timeChanged(t);
    }
}
