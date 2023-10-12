#include <QPainter>
#include <QApplication>
#include <QTextDocument>
#include <QStyleOptionViewItem>
#include <QAbstractTextDocumentLayout>
#include "htmldelegate.h"

///
/// \brief HtmlDelegate::HtmlDelegate
/// \param parent
///
HtmlDelegate::HtmlDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

///
/// \brief HtmlDelegate::paint
/// \param painter
/// \param option
/// \param index
///
void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (opt.text.isEmpty()) {
        // This is nothing this function is supposed to handle
        QStyledItemDelegate::paint(painter, option, index);

        return;
    }

    QStyle *style = opt.widget? opt.widget->style() : QApplication::style();

    QTextOption textOption;
    textOption.setWrapMode(opt.features & QStyleOptionViewItem::WrapText ? QTextOption::WordWrap
                                                                         : QTextOption::ManualWrap);
    textOption.setTextDirection(opt.direction);

    QTextDocument doc;
    doc.setHtml(opt.text);
    doc.setDocumentMargin(2);
    doc.setDefaultFont(opt.font);
    doc.setDefaultTextOption(textOption);
    doc.setTextWidth(opt.rect.width());

    /// Painting item without text
    opt.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (opt.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, opt.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

///
/// \brief HtmlDelegate::sizeHint
/// \param option
/// \param index
/// \return
///
QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (opt.text.isEmpty()) {
        // This is nothing this function is supposed to handle
        return QStyledItemDelegate::sizeHint(option, index);
    }

    QTextDocument doc;
    doc.setHtml(opt.text);
    doc.setDocumentMargin(2);
    doc.setDefaultFont(opt.font);

    if(opt.features & QStyleOptionViewItem::WrapText)
        doc.setTextWidth(opt.rect.width());

    return QSize(doc.idealWidth(), doc.size().height());
}
