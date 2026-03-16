#include <QApplication>
#include <QPainter>
#include <QStyleOptionViewItem>
#include "datadelegate.h"

///
/// \brief DataDelegate::paint
/// \param painter
/// \param option
/// \param index
///
void DataDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const auto color = index.data(Qt::UserRole + 7).value<QColor>();
    if ( color.isValid() )
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        auto style = opt.widget ? opt.widget->style() : QApplication::style();
        auto text = opt.text.trimmed();
        const auto descr_index = text.indexOf(';');
        if (descr_index > 0)
        {
            text = text.left(descr_index);
        }

        QFontMetrics fm(opt.font);
        auto rect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
        rect.setWidth(style->itemTextRect(fm, opt.rect, opt.displayAlignment, true, text).width());
        rect.adjust(2, 1, 2, -1);
        painter->fillRect(rect, color);
    }

    QStyledItemDelegate::paint(painter, option, index);
}
