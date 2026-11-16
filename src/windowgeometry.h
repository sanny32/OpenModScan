#ifndef WINDOWGEOMETRY_H
#define WINDOWGEOMETRY_H

#include <QList>
#include <QRect>

// Preserve windows with a reachable title bar, including windows spanning screens.
// Screen rectangles must be availableGeometry(), excluding taskbars and docks.
inline QRect visibleWindowGeometry(const QRect& geometry, const QList<QRect>& screens,
                                   const QRect& primaryScreen)
{
    const QRect titleArea(geometry.topLeft(), QSize(geometry.width(), qMin(30, geometry.height())));
    for (const QRect& screen : screens) {
        const QRect visibleTitle = titleArea.intersected(screen);
        if (visibleTitle.width() >= qMin(100, geometry.width())
            && visibleTitle.height() == titleArea.height())
            return geometry;
    }

    if (!primaryScreen.isValid())
        return geometry;

    QRect result(QPoint(), geometry.size().boundedTo(primaryScreen.size()));
    result.moveTopLeft(primaryScreen.topLeft()
                       + QPoint((primaryScreen.width() - result.width()) / 2,
                                (primaryScreen.height() - result.height()) / 2));
    return result;
}

#endif // WINDOWGEOMETRY_H
