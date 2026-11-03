#ifndef DATADELEGATE_H
#define DATADELEGATE_H

#include <QColor>
#include <QStyledItemDelegate>

///
/// \brief The DataDelegate class
///
class DataDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DataDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    QColor addressColor() const { return _addressColor; }
    void setAddressColor(const QColor& c) { _addressColor = c; }

    QColor commentColor() const { return _commentColor; }
    void setCommentColor(const QColor& c) { _commentColor = c; }

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    QColor _addressColor{ 128, 128, 128 };
    QColor _commentColor{ 128, 128, 128 };
};

#endif // DATADELEGATE_H
