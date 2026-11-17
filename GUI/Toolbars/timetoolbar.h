#pragma once
#include <QToolBar>

class QToolButton;

class TimeToolBar : public QToolBar {
    Q_OBJECT
public:
    explicit TimeToolBar(QWidget* parent = nullptr);
    void  setTime(float seconds);
    float time() const { return m_seconds; }

signals:
    void timeChanged(float seconds);

private slots:
    void onButtonClicked();

private:
    QToolButton* m_button = nullptr;
    float m_seconds = 0.0f;
};
