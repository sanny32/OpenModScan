#ifndef DIALOGMBMAPVIEWS_H
#define DIALOGMBMAPVIEWS_H

#include <QDialog>
#include "mbmap.h"

class QLineEdit;
class QPushButton;
class QTreeWidget;

///
/// \brief The DialogMbmapViews class picks which blocks of a large device
/// map open as windows
///
class DialogMbmapViews : public QDialog
{
    Q_OBJECT

public:
    DialogMbmapViews(const MbMap& map, const QList<MbMap::View>& views, int preselected, QWidget* parent = nullptr);

    /// The blocks to open, in the order of the map
    QList<MbMap::View> selectedViews() const;

private slots:
    void on_filterChanged(const QString& text);
    void updateOpenButton();

private:
    void checkVisible(bool on);

private:
    const QList<MbMap::View> _views;
    QLineEdit* _filter;
    QTreeWidget* _list;
    QPushButton* _openButton;
};

#endif // DIALOGMBMAPVIEWS_H
