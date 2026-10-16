#ifndef DIALOGMBMAPVIEWS_H
#define DIALOGMBMAPVIEWS_H

#include <QDialog>
#include "mbmap.h"

class QPushButton;
class QTreeWidgetItem;

namespace Ui {
class DialogMbmapViews;
}

///
/// \brief The DialogMbmapViews class picks which blocks of a large device
/// map open as windows
///
class DialogMbmapViews : public QDialog
{
    Q_OBJECT

public:
    ///
    /// \brief Lists the blocks of a map, each with a check box
    /// \param map The map the blocks come from, for the title and the address base
    /// \param views The blocks, as MbMap::views() lays them out
    /// \param preselected How many blocks, from the first, are checked at first
    /// \param parent Parent widget
    ///
    DialogMbmapViews(const MbMap& map, const QList<MbMap::View>& views, int preselected, QWidget* parent = nullptr);
    ~DialogMbmapViews() override;

    ///
    /// \brief The blocks to open
    /// \return The checked blocks, in the order of the map
    ///
    QList<MbMap::View> selectedViews() const;

private slots:
    void on_lineEditFilter_textChanged(const QString& text);
    void on_treeWidget_itemChanged(QTreeWidgetItem* item, int column);
    void on_pushButtonCheckShown_clicked();
    void on_pushButtonUncheckShown_clicked();

private:
    void checkVisible(bool on);
    void updateOpenButton();

private:
    Ui::DialogMbmapViews* ui;
    const QList<MbMap::View> _views;
    QPushButton* _openButton = nullptr;
};

#endif // DIALOGMBMAPVIEWS_H
