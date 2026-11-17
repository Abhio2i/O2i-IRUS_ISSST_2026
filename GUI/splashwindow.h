#ifndef SPLASHWINDOW_H
#define SPLASHWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QString>

class SplashWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SplashWindow(const QPixmap &background,
                          const QString &appName,
                          const QString &version,
                          const QString &copyright,
                          QWidget *parent = nullptr);

    void setProgress(int value, const QString &message);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_bg;
    QString m_appName;
    QString m_version;
    QString m_copyright;
    QString m_message;
    int     m_progress = 0;
};

#endif // SPLASHWINDOW_H
