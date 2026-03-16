#ifndef DATADELEGATE_H
#define DATADELEGATE_H

#include <QStyledItemDelegate>

///
/// \brief The DataDelegate class
///
class DataDelegate : public QStyledItemDelegate
{
public:
    explicit DataDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // DATADELEGATE_H
