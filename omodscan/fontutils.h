#ifndef FONTUTILS_H
#define FONTUTILS_H

#include <QFont>
#include <QList>
#include <QFontDatabase>

inline QFont defaultMonospaceFont(int pointSize = -1)
{
    QString family;

#if defined(Q_OS_WIN)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto families = QFontDatabase::families();
#else
    auto families = QFontDatabase().families();
#endif
    if (families.contains("Consolas"))
        family = "Consolas";
    else
        family = "Courier New";

#elif defined(Q_OS_MAC)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto families = QFontDatabase::families();
#else
    auto families = QFontDatabase().families();
#endif
    if (families.contains("Menlo"))
        family = "Menlo";
    else
        family = "Monaco";

#else
    family = "Monospace";
#endif

    QFont font(family, pointSize);
    font.setStyleHint(QFont::Monospace);
    return font;
}

#endif // FONTUTILS_H
