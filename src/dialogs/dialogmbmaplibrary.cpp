#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QPushButton>
#include <QStandardPaths>
#include "mbmap.h"
#include "dialogmbmaplibrary.h"
#include "ui_dialogmbmaplibrary.h"

namespace {

const QString LibraryUrl = QStringLiteral("https://raw.githubusercontent.com/nobrega8/mbmap-library/main/");
constexpr int PathRole = Qt::UserRole + 1;

}

///
/// \brief Opens the dialog and starts downloading the list of maps
/// \param parent Parent widget
///
DialogMbmapLibrary::DialogMbmapLibrary(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogMbmapLibrary)
{
    ui->setupUi(this);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);

    _openButton = ui->buttonBox->addButton(tr("Open"), QDialogButtonBox::AcceptRole);
    _openButton->setEnabled(false);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogMbmapLibrary::on_open);

    setBusy(true, tr("Loading the list of maps..."));
    auto reply = _network.get(QNetworkRequest(QUrl(LibraryUrl + "index.json")));
    connect(reply, &QNetworkReply::finished, this, &DialogMbmapLibrary::on_indexFinished);
}

///
/// \brief Deletes the dialog's form
///
DialogMbmapLibrary::~DialogMbmapLibrary()
{
    delete ui;
}

///
/// \brief Shows what the dialog is doing, and locks the list while it waits
/// \param busy true while a download runs
/// \param status Text for the status line
///
void DialogMbmapLibrary::setBusy(bool busy, const QString& status)
{
    ui->treeWidget->setEnabled(!busy);
    _openButton->setEnabled(!busy && !ui->treeWidget->selectedItems().isEmpty());
    ui->labelStatus->setText(status);
}

///
/// \brief Fills the list from the library's index.json
///
void DialogMbmapLibrary::on_indexFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if(reply->error() != QNetworkReply::NoError) {
        setBusy(false, tr("The list of maps could not be loaded: %1").arg(reply->errorString()));
        return;
    }

    const auto maps = QJsonDocument::fromJson(reply->readAll()).object().value("maps").toArray();
    ui->treeWidget->setSortingEnabled(false);
    for(auto&& value : maps)
    {
        const auto entry = value.toObject();
        const auto path = entry.value("path").toString();
        if(MbMap::cachedFilePath(QStringLiteral("/mbmap-library"), path).isEmpty()) continue;

        auto item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, entry.value("vendor").toString());
        item->setText(1, entry.value("model").toString());
        item->setText(2, entry.value("description").toString());
        item->setData(3, Qt::DisplayRole, entry.value("registers").toInt());
        item->setData(0, PathRole, path);
    }
    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    on_lineEditFilter_textChanged(ui->lineEditFilter->text());

    setBusy(false, tr("%n map(s)", nullptr, ui->treeWidget->topLevelItemCount()));
}

///
/// \brief Shows only the maps whose vendor, model or description contain
/// every word of the text
/// \param text The filter, matched without case
///
void DialogMbmapLibrary::on_lineEditFilter_textChanged(const QString& text)
{
    const auto words = text.split(' ', Qt::SkipEmptyParts);
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
        auto item = ui->treeWidget->topLevelItem(i);
        const auto haystack = QString("%1 %2 %3").arg(item->text(0), item->text(1), item->text(2));
        bool match = true;
        for(auto&& w : words)
            match = match && haystack.contains(w, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

///
/// \brief Enables Open when a map is selected
///
void DialogMbmapLibrary::on_treeWidget_itemSelectionChanged()
{
    _openButton->setEnabled(!ui->treeWidget->selectedItems().isEmpty());
}

///
/// \brief Opens the map that was double-clicked
/// \param item The map's row
/// \param column The column clicked
///
void DialogMbmapLibrary::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(item)
    Q_UNUSED(column)
    on_open();
}

///
/// \brief Downloads the selected map
///
void DialogMbmapLibrary::on_open()
{
    const auto selected = ui->treeWidget->selectedItems();
    if(selected.isEmpty()) return;

    const auto path = selected.first()->data(0, PathRole).toString();
    setBusy(true, tr("Downloading %1...").arg(QFileInfo(path).fileName()));

    auto reply = _network.get(QNetworkRequest(QUrl(LibraryUrl + path)));
    reply->setProperty("path", path);
    connect(reply, &QNetworkReply::finished, this, &DialogMbmapLibrary::on_mapFinished);
}

///
/// \brief Checks and saves the downloaded map, then closes the dialog
///
void DialogMbmapLibrary::on_mapFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if(reply->error() != QNetworkReply::NoError) {
        setBusy(false, tr("The map could not be downloaded: %1").arg(reply->errorString()));
        return;
    }

    const auto data = reply->readAll();
    QString error;
    MbMap::parse(data, &error);
    if(!error.isEmpty()) {
        setBusy(false, error);
        return;
    }

    // Kept next to the app's other data, so the map can be opened again
    // from the recent files
    const auto path = reply->property("path").toString();
    const auto filename = MbMap::cachedFilePath(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mbmap-library", path);
    if(filename.isEmpty()) {
        setBusy(false, tr("The map has an invalid path in the library"));
        return;
    }
    QDir().mkpath(QFileInfo(filename).absolutePath());

    QFile file(filename);
    if(!file.open(QFile::WriteOnly) || file.write(data) != data.size()) {
        setBusy(false, tr("The map could not be saved: %1").arg(file.errorString()));
        return;
    }

    _filename = filename;
    accept();
}
