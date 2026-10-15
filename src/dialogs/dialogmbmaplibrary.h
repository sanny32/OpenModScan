#ifndef DIALOGMBMAPLIBRARY_H
#define DIALOGMBMAPLIBRARY_H

#include <QDialog>
#include <QNetworkAccessManager>

class QLabel;
class QLineEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

///
/// \brief The DialogMbmapLibrary class lists the device maps of the
/// mbmap-library repository and downloads the one picked
///
class DialogMbmapLibrary : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMbmapLibrary(QWidget* parent = nullptr);

    /// The downloaded map, once the dialog is accepted
    QString filename() const {
        return _filename;
    }

private slots:
    void on_indexFinished();
    void on_mapFinished();
    void on_filterChanged(const QString& text);
    void on_open();

private:
    void setBusy(bool busy, const QString& status);

private:
    QNetworkAccessManager _network;
    QLineEdit* _filter;
    QTreeWidget* _list;
    QLabel* _status;
    QPushButton* _openButton;
    QString _filename;
};

#endif // DIALOGMBMAPLIBRARY_H
