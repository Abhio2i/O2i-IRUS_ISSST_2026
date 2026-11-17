#include "splashwindow.h"
#include <QPainter>
#include <QScreen>
#include <QGuiApplication>
#include <QCoreApplication>

SplashWindow::SplashWindow(const QPixmap &background,
                           const QString &appName,
                           const QString &version,
                           const QString &copyright,
                           QWidget *parent)
    : QWidget(parent, Qt::SplashScreen | Qt::FramelessWindowHint)
    , m_bg(background)
    , m_appName(appName)
    , m_version(version)
    , m_copyright(copyright)
{
    setFixedSize(760, 530);
    setAttribute(Qt::WA_TranslucentBackground, false);

    if (QScreen *s = QGuiApplication::primaryScreen()) {
        move(s->availableGeometry().center() - rect().center());
    }
}

void SplashWindow::setProgress(int value, const QString &message)
{
    m_progress = qBound(0, value, 100);
    m_message  = message;
    repaint();
    QCoreApplication::processEvents();
}

void SplashWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                     QPainter::SmoothPixmapTransform);

    const int W = width();
    const int H = height();
    const int imgH = 355;

    p.fillRect(rect(), QColor("#000000"));

    QRect imgRect(0, 0, W, imgH);
    if (!m_bg.isNull()) {
        QPixmap scaled = m_bg.scaled(imgRect.size(),
                                     Qt::KeepAspectRatioByExpanding,
                                     Qt::SmoothTransformation);
        int sx = (scaled.width()  - imgRect.width())  / 2;
        int sy = (scaled.height() - imgRect.height()) / 2;
        p.drawPixmap(imgRect, scaled, QRect(sx, sy, imgRect.width(), imgRect.height()));
    } else {
        p.fillRect(imgRect, QColor("#0F2636"));
    }

    QFont titleFont = font();
    titleFont.setPixelSize(42);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.setPen(Qt::white);
    p.drawText(QRect(0, imgH - 140, W, 60), Qt::AlignCenter, m_appName);

    const int barH = 5;
    p.fillRect(0, imgH, W, barH, QColor("#1A3A4F"));
    p.fillRect(0, imgH, W * m_progress / 100, barH, QColor("#00BFFF"));

    QFont small = font();
    small.setPixelSize(12);
    p.setFont(small);
    p.setPen(QColor("#B0B0B0"));
    p.drawText(QRect(20, imgH + 55, W - 40, 20),
               Qt::AlignRight | Qt::AlignVCenter, m_message);

    p.setPen(Qt::white);
    p.drawText(QRect(20, imgH + 80, W - 40, 20),
               Qt::AlignRight | Qt::AlignVCenter, m_version);

    p.setPen(QColor("#B0B0B0"));
    p.drawText(QRect(20, H - 45, W - 200, 20),
               Qt::AlignLeft | Qt::AlignVCenter, m_copyright);

    QFont big = font();
    big.setPixelSize(16);
    big.setBold(true);
    p.setFont(big);
    p.setPen(Qt::white);
    p.drawText(QRect(W - 120, H - 50, 100, 28),
               Qt::AlignRight | Qt::AlignVCenter,
               QString("%1%").arg(m_progress));
}
