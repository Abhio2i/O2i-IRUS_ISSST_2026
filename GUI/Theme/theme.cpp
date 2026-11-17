#include "theme.h"
#include <QSettings>
#include <QApplication>
#include <QWidget>
#include <QEvent>
#include <QPalette>
#include <QPair>
#include <QList>
#include <QSet>
#include <QDebug>
#include <QColor>
#include <QRegularExpression>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager()
{
    registerThemes();
    QSettings settings;
    m_currentId = settings.value("Appearance/Theme", "dark").toString();
}

void ThemeManager::registerThemes()
{
    m_themes = {
        ThemeColorsData::Dark,
        ThemeColorsData::Black,
        ThemeColorsData::White
    };
}

QString ThemeManager::currentName() const
{
    for (const auto& t : m_themes)
        if (t.id == m_currentId) return t.displayName;
    return "Dark";
}

const ThemeColors& ThemeManager::colors() const
{
    for (const auto& t : m_themes)
        if (t.id == m_currentId) return t;
    return ThemeColorsData::Dark;
}

void ThemeManager::setTheme(const QString& themeId)
{
    bool found = false;
    for (const auto& t : m_themes) {
        if (t.id == themeId) { found = true; break; }
    }
    if (!found || m_currentId == themeId)
        return;

    m_currentId = themeId;
    m_cache.clear();
    QSettings settings;
    settings.setValue("Appearance/Theme", m_currentId);

    applyPalette();
    emit themeChanged(m_currentId);
}

void ThemeManager::setThemeByName(const QString& displayName)
{
    for (const auto& t : m_themes) {
        if (t.displayName.compare(displayName, Qt::CaseInsensitive) == 0) {
            setTheme(t.id);
            return;
        }
    }
}

// ───────────────────────── Auto theming ─────────────────────────

static bool isTextProperty(const QString& s, int pos)
{
    int start = qMax(qMax(s.lastIndexOf(QChar(';'), pos), s.lastIndexOf(QChar('{'), pos)),
                     s.lastIndexOf(QChar('}'), pos));
    const QString decl = s.mid(start + 1, pos - start - 1);
    const QString prop = decl.section(':', 0, 0).trimmed().toLower();
    return prop == "color" || prop == "selection-color";
}

static QColor adaptOne(const QColor& col, bool textCtx, bool lightTarget, const ThemeColors& c)
{
    qreal h = 0.0, s = 0.0, l = 0.0;
    col.getHslF(&h, &s, &l);
    if (h < 0.0)
        h = 0.0;

    if (lightTarget) {
        if (textCtx)
            return (l > 0.65) ? QColor(c.text) : col;

        if (l < 0.35)
            return QColor::fromHslF(h, qMin(s, 0.30), 0.97 - l * 0.4);

        return col;
    }

    if (!textCtx && l < 0.35 && s > 0.10)
        return QColor::fromHslF(h, s * 0.10, l);

    return col;
}

QString ThemeManager::adaptUnmapped(const QString& s) const
{
    const ThemeColors& c = colors();
    const bool lightTarget = QColor(c.background).lightnessF() > 0.5;

    static const QRegularExpression hexRe("#(?:[0-9a-fA-F]{6}|[0-9a-fA-F]{3})\\b");
    QString out;
    out.reserve(s.size());
    int last = 0;
    auto it = hexRe.globalMatch(s);
    while (it.hasNext()) {
        const auto m = it.next();
        out += s.mid(last, m.capturedStart() - last);
        last = m.capturedEnd();

        const QColor src(m.captured(0));
        const QColor dst = adaptOne(src, isTextProperty(s, m.capturedStart()), lightTarget, c);
        out += (dst == src) ? m.captured(0) : dst.name();
    }
    out += s.mid(last);
    return out;
}

static QString ensureContrast(const QString& qss, const ThemeColors& c)
{
    static const QRegularExpression blockRe("\\{([^{}]*)\\}");
    static const QRegularExpression bgRe(
        "(?<![-\\w])background(?:-color)?\\s*:\\s*(#(?:[0-9a-fA-F]{6}|[0-9a-fA-F]{3}))\\b");
    static const QRegularExpression fgRe(
        "(?<![-\\w])color\\s*:\\s*(#(?:[0-9a-fA-F]{6}|[0-9a-fA-F]{3}))\\b");

    QString out;
    out.reserve(qss.size());
    int last = 0;
    auto it = blockRe.globalMatch(qss);
    while (it.hasNext()) {
        const auto m = it.next();
        QString body = m.captured(1);

        const auto bm = bgRe.match(body);
        const auto fm = fgRe.match(body);
        if (bm.hasMatch() && fm.hasMatch()) {
            const QColor bgc(bm.captured(1)), fgc(fm.captured(1));
            if (qAbs(bgc.lightnessF() - fgc.lightnessF()) < 0.35) {
                const QString fix = bgc.lightnessF() > 0.5 ? c.text : QString("#FFFFFF");
                body.replace(fm.capturedStart(1), fm.capturedLength(1), fix);
            }
        }
        out += qss.mid(last, m.capturedStart(1) - last);
        out += body;
        last = m.capturedEnd(1);
    }
    out += qss.mid(last);
    return out;
}

QString ThemeManager::translate(const QString& qss) const
{
    const ThemeColors& d = ThemeColorsData::Dark;
    const ThemeColors& c = colors();
    if (c.id == d.id)
        return qss;

    const auto cached = m_cache.constFind(qss);
    if (cached != m_cache.constEnd())
        return cached.value();

    const QList<QPair<QString, QString>> map = {
                                                {d.background,    c.background},     // #0F2636
                                                {d.secondary,     c.secondary},      // #1A3652
                                                {d.border,        c.border},         // #27446d
                                                {d.textSecondary, c.textSecondary},  // #B0C4D8
                                                {d.hover,         c.hover},          // #203E5A
                                                {d.accent,        c.accent},         // #0078D4
                                                {d.accentHover,   c.accentHover},    // #106EBE
                                                {d.error,         c.error},          // #F44336
                                                {"#FFFFFF",       c.text},
                                                {"#1A3A4F",       c.hover},
                                                {"#0090CC",       c.accentHover},
                                                {"#162F47",       c.secondary},
                                                {"#1E3E5E",       c.hover},
                                                {"#3A5A7A",       c.border},
                                                {"#E0E0E0",       c.text},
                                                {"#B0B0B0",       c.textSecondary},
                                                {"#C0C0C0",       c.textSecondary},
                                                {"#00BFFF",       c.accent},
                                                };

    QString out = qss;
    for (int i = 0; i < map.size(); ++i)
        out.replace(map[i].first, QString("@@%1@@").arg(i), Qt::CaseInsensitive);

    const QColor bg(c.background);
    static const QRegularExpression rgbaBg(
        "rgba\\(\\s*15\\s*,\\s*38\\s*,\\s*54\\s*,");
    out.replace(rgbaBg, QString("rgba(%1, %2, %3,")
                            .arg(bg.red()).arg(bg.green()).arg(bg.blue()));

    out = adaptUnmapped(out);

    for (int i = 0; i < map.size(); ++i)
        out.replace(QString("@@%1@@").arg(i), map[i].second);

    static const QRegularExpression whiteText(
        "(?<![-\\w])color\\s*:\\s*(white|#fff(?:fff)?)\\b",
        QRegularExpression::CaseInsensitiveOption);
    out.replace(whiteText, QString("color: %1").arg(c.text));

    out = ensureContrast(out, c);

#ifndef QT_NO_DEBUG_OUTPUT
    static QSet<QString> reported;
    static const QRegularExpression hexRe("#[0-9a-fA-F]{6}\\b|#[0-9a-fA-F]{3}\\b");
    QSet<QString> known;
    for (const auto& p : map) known.insert(p.first.toLower());
    auto it = hexRe.globalMatch(qss);
    while (it.hasNext()) {
        const QString h = it.next().captured(0).toLower();
        if (!known.contains(h) && !reported.contains(h)) {
            reported.insert(h);
            qDebug() << "[Theme] auto-adapted unmapped color:" << h;
        }
    }
#endif

    m_cache.insert(qss, out);
    return out;
}

void ThemeManager::retheme(QWidget* w)
{
    if (!w || w->property("_noTheme").toBool())
        return;

    const QString cur  = w->styleSheet();
    const QString last = w->property("_lastQss").toString();
    QString orig       = w->property("_origQss").toString();

    if (cur != last) {
        if (cur.isEmpty()) {
            w->setProperty("_origQss", QVariant());
            w->setProperty("_lastQss", QVariant());
            return;
        }
        orig = cur;
        w->setProperty("_origQss", orig);
    }
    if (orig.isEmpty()) return;

    const QString t = translate(orig);
    w->setProperty("_lastQss", t);
    if (t != cur)
        w->setStyleSheet(t);
}

bool ThemeManager::eventFilter(QObject* o, QEvent* e)
{
    const QEvent::Type t = e->type();
    if (t == QEvent::Polish || t == QEvent::StyleChange) {
        if (auto* w = qobject_cast<QWidget*>(o))
            retheme(w);
    }
    return QObject::eventFilter(o, e);
}

void ThemeManager::applyPalette()
{
    const ThemeColors& c = colors();

    QPalette pal;
    pal.setColor(QPalette::Window,          QColor(c.background));
    pal.setColor(QPalette::WindowText,      QColor(c.text));
    pal.setColor(QPalette::Base,            QColor(c.secondary));
    pal.setColor(QPalette::AlternateBase,   QColor(c.background));
    pal.setColor(QPalette::Text,            QColor(c.text));
    pal.setColor(QPalette::Button,          QColor(c.secondary));
    pal.setColor(QPalette::ButtonText,      QColor(c.text));
    pal.setColor(QPalette::BrightText,      Qt::red);
    pal.setColor(QPalette::ToolTipBase,     QColor(c.secondary));
    pal.setColor(QPalette::ToolTipText,     QColor(c.text));
    pal.setColor(QPalette::Link,            QColor(c.accent));
    pal.setColor(QPalette::Highlight,       QColor(c.accent));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(pal);

    //     const QString tooltip = R"(
    //         QToolTip {
    //             background-color: #1A3652;
    //             color: white;
    //             border: 1px solid #0078D4;
    //             border-radius: 2px;
    //             padding: 2px 4px;
    //             font-size: 11px;
    //         }
    //     )";
    //     qApp->setStyleSheet(translate(tooltip));
    // }
    const QString tooltip = R"(
        QToolTip {
            background-color: #1A3652;
            color: white;
            border: 1px solid #0078D4;
            border-radius: 2px;
            padding: 2px 4px;
            font-size: 11px;
        }
    )";

    // Checkbox ka indicator: border hamesha theme ke visible color mein
    const QString checkStyle = QString(R"(
        QCheckBox::indicator {
            width: 14px;
            height: 14px;
            border: 2px solid %1;
            border-radius: 3px;
            background-color: %2;
        }
        QCheckBox::indicator:hover {
            border-color: %3;
        }
        QCheckBox::indicator:checked {
            background-color: %3;
            border-color: %3;
            image: url(:/icons/images/check.png);
        }
        QCheckBox::indicator:disabled {
            border-color: %4;
        }
    )").arg(c.textSecondary, c.background, c.accent, c.disabled);

    qApp->setStyleSheet(translate(tooltip) + checkStyle);
}

void ThemeManager::installAutoTheme()
{
    qApp->installEventFilter(this);
    connect(this, &ThemeManager::themeChanged, this, [this]() {
        for (QWidget* w : qApp->allWidgets())
            retheme(w);
    });
}
