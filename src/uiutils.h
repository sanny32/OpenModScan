#ifndef UIUTILS_H
#define UIUTILS_H

#include <QColor>
#include <QPainter>
#include <QPushButton>

inline void recolorPushButtonIcon(QPushButton* btn, const QColor& color)
{
    if (!btn) return;

    QIcon origIcon = btn->icon();
    if (origIcon.isNull()) return;

    QSize iconSize = btn->iconSize();
    if (!iconSize.isValid())
        iconSize = btn->size();

    QPixmap pixmap = origIcon.pixmap(iconSize);
    if (pixmap.isNull()) return;

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    btn->setIcon(QIcon(pixmap));
}

#endif // UIUTILS_H
