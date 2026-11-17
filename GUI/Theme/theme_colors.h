#ifndef THEME_COLORS_H
#define THEME_COLORS_H

#include <QString>

struct ThemeColors
{
    QString id;
    QString displayName;
    QString background;
    QString secondary;
    QString border;
    QString text;
    QString textSecondary;
    QString accent;
    QString accentHover;
    QString error;
    QString hover;
    QString selected;
    QString disabled;
    QString previewBg;
    QString previewBorder;
    QString previewText;
};

namespace ThemeColorsData
{

inline const ThemeColors Dark = {
    "dark", "Dark",
    "#0F2636",   // background
    "#1A3652",   // secondary
    "#27446d",   // border
    "#FFFFFF",   // text
    "#B0C4D8",   // textSecondary
    "#0078D4",   // accent
    "#106EBE",   // accentHover
    "#F44336",   // error
    "#203E5A",   // hover
    "#0078D4",   // selected
    "#4A4A4A",   // disabled
    "#0F2636",   // previewBg
    "#27446d",   // previewBorder
    "#FFFFFF"    // previewText
};

inline const ThemeColors Black = {
    "black", "Black",
    "#0A0A0A", "#1A1A1A", "#333333", "#FFFFFF", "#CCCCCC",
    "#0078D4", "#106EBE", "#FF6B6B", "#2A2A2A", "#0078D4", "#3A3A3A",
    "#0A0A0A", "#333333", "#FFFFFF"
};

inline const ThemeColors White = {
    "white", "White",
    "#F5F7FA", "#FFFFFF", "#C5D0DE", "#1A2A3A", "#5A6A7A",
    "#0078D4", "#106EBE", "#D13438", "#D5DEE9", "#0078D4", "#D0D0D0",
    "#F5F7FA", "#C5D0DE", "#1A2A3A"
};

}

#endif // THEME_COLORS_H
