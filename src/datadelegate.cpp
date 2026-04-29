#include <QApplication>
#include <QPainter>
#include <QStyleOptionViewItem>
#include "datadelegate.h"
#include "outputwidget.h"

///
/// \brief DataDelegate::paint
/// \param painter
/// \param option
/// \param index
///
void DataDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QColor color = index.data(ColorRole).value<QColor>();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const auto* widget = opt.widget;
    const auto* style  = widget ? widget->style() : QApplication::style();
    const QFontMetrics fm(opt.font);
    const QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);

    // 1. Draw item background (selection highlight, hover, etc.) without text.
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    // 2. Draw decoration (simulation icon).
    if (!opt.icon.isNull())
    {
        const QRect decorationRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
        opt.icon.paint(painter, decorationRect, opt.decorationAlignment);
    }

    // 3. Draw custom colored background (for highlighted cells) on top.
    if (color.isValid())
    {
        auto text = opt.text.trimmed();
        const int di = text.indexOf(';');
        if (di > 0) text = text.left(di);

        QRect colorRect = textRect;
        colorRect.setWidth(style->itemTextRect(fm, opt.rect, opt.displayAlignment, true, text).width());
        painter->fillRect(colorRect, color);
    }

    // 4. Draw text parts with different colors (single pass, no double rendering).
    const bool selected = opt.state & QStyle::State_Selected;
    const QPalette& pal = opt.palette;
    const QColor mainColor = selected ? pal.color(QPalette::HighlightedText) : pal.color(QPalette::Text);
    const QColor dimColor  = selected ? pal.color(QPalette::HighlightedText) : _addressColor;

    const QString fullText = opt.text;
    const int colonPos = fullText.indexOf(QLatin1String(": "));
    const int descrPos = fullText.indexOf(QLatin1Char(';'));

    const QString addrPart  = (colonPos >= 0) ? fullText.left(colonPos + 2) : QString();
    const QString valuePart = (colonPos >= 0)
                                  ? ((descrPos > colonPos) ? fullText.mid(colonPos + 2, descrPos - colonPos - 2)
                                                           : fullText.mid(colonPos + 2))
                                  : fullText;
    const QString descrPart = (descrPos > 0) ? fullText.mid(descrPos) : QString();

    painter->save();
    painter->setFont(opt.font);
    painter->setClipRect(textRect);

    int x = textRect.x();
    const int y = textRect.y();
    const int h = textRect.height();

    auto drawPart = [&](const QString& text, const QColor& c)
    {
        if (text.isEmpty()) return;
        const int w = fm.horizontalAdvance(text);
        painter->setPen(c);
        painter->drawText(QRect(x, y, w, h), Qt::AlignVCenter | Qt::TextSingleLine, text);
        x += w;
    };

    const QColor commentColor = selected ? pal.color(QPalette::HighlightedText) : _commentColor;

    drawPart(addrPart,  dimColor);
    drawPart(valuePart, mainColor);
    drawPart(descrPart, commentColor);

    painter->restore();

    // 5. Draw focus rect if needed.
    if (opt.state & QStyle::State_HasFocus)
    {
        QStyleOptionFocusRect focusOpt;
        focusOpt.QStyleOption::operator=(opt);
        focusOpt.rect = style->subElementRect(QStyle::SE_ItemViewItemFocusRect, &opt, widget);
        style->drawPrimitive(QStyle::PE_FrameFocusRect, &focusOpt, painter, widget);
    }
}
