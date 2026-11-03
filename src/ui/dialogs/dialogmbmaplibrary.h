#ifndef DIALOGMBMAPLIBRARY_H
#define DIALOGMBMAPLIBRARY_H

#include <QDialog>
#include <QNetworkAccessManager>

class QPushButton;
class QTreeWidgetItem;

namespace Ui {
class DialogMbmapLibrary;
}

///
/// \brief The DialogMbmapLibrary class lists the device maps of the
/// mbmap-library repository and downloads the one picked
///
class DialogMbmapLibrary : public QDialog
{
    Q_OBJECT

public:
    ///
    /// \brief Opens the dialog and starts downloading the list of maps
    /// \param parent Parent widget
    ///
    explicit DialogMbmapLibrary(QWidget* parent = nullptr);
    ~DialogMbmapLibrary() override;

    ///
    /// \brief The downloaded map
    /// \return Path of the saved .mbmap.json file once the dialog is accepted,
    /// empty before
    ///
    QString filename() const {
        return _filename;
    }

private slots:
    void on_indexFinished();
    void on_mapFinished();
    void on_lineEditFilter_textChanged(const QString& text);
    void on_treeWidget_itemSelectionChanged();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void on_open();

private:
    void setBusy(bool busy, const QString& status);

private:
    Ui::DialogMbmapLibrary* ui;
    QNetworkAccessManager _network;
    QPushButton* _openButton = nullptr;
    QString _filename;
};

#endif // DIALOGMBMAPLIBRARY_H
