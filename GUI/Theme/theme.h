#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include "theme_colors.h"

class QWidget;
class QEvent;
class QColor;

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    static ThemeManager& instance();

    QString currentId() const { return m_currentId; }
    QString currentName() const;
    const ThemeColors& colors() const;
    const QVector<ThemeColors>& availableThemes() const { return m_themes; }

    void setTheme(const QString& themeId);
    void setThemeByName(const QString& displayName);

    void installAutoTheme();
    void applyPalette();
    QString translate(const QString& qss) const;

signals:
    void themeChanged(const QString& themeId);

protected:
    bool eventFilter(QObject* o, QEvent* e) override;

private:
    ThemeManager();
    void registerThemes();
    void retheme(QWidget* w);
    QString adaptUnmapped(const QString& qss) const;

    QString m_currentId = "dark";
    QVector<ThemeColors> m_themes;
    mutable QHash<QString, QString> m_cache;
};

#endif // THEME_H
