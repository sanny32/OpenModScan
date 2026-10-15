#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkReply>
#include <QPushButton>
#include <QStandardPaths>
#include <QTreeWidget>
#include <QVBoxLayout>
#include "mbmap.h"
#include "dialogmbmaplibrary.h"

namespace {

const QString LibraryUrl = QStringLiteral("https://raw.githubusercontent.com/nobrega8/mbmap-library/main/");
constexpr int PathRole = Qt::UserRole + 1;

}

///
/// \brief DialogMbmapLibrary::DialogMbmapLibrary
/// \param parent
///
DialogMbmapLibrary::DialogMbmapLibrary(QWidget* parent)
    : QDialog(parent)
    ,_filter(new QLineEdit(this))
    ,_list(new QTreeWidget(this))
    ,_status(new QLabel(this))
{
    setWindowTitle(tr("Device Map Library"));
    resize(720, 480);

    _filter->setPlaceholderText(tr("Filter by vendor, model or description"));
    _filter->setClearButtonEnabled(true);

    _list->setRootIsDecorated(false);
    _list->setAlternatingRowColors(true);
    _list->setSortingEnabled(true);
    _list->setHeaderLabels({ tr("Vendor"), tr("Model"), tr("Description"), tr("Registers") });
    _list->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    _list->sortByColumn(0, Qt::AscendingOrder);

    auto source = new QLabel(tr("Maps from <a href=\"https://github.com/nobrega8/mbmap-library\">mbmap-library</a>. "
                                "Opening one creates a window for each block of registers."), this);
    source->setOpenExternalLinks(true);
    source->setWordWrap(true);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    _openButton = buttons->addButton(tr("Open"), QDialogButtonBox::AcceptRole);
    _openButton->setEnabled(false);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(_filter);
    layout->addWidget(_list);
    layout->addWidget(source);
    layout->addWidget(_status);
    layout->addWidget(buttons);

    connect(_filter, &QLineEdit::textChanged, this, &DialogMbmapLibrary::on_filterChanged);
    connect(_list, &QTreeWidget::itemSelectionChanged, this, [this] {
        _openButton->setEnabled(!_list->selectedItems().isEmpty());
    });
    connect(_list, &QTreeWidget::itemDoubleClicked, this, &DialogMbmapLibrary::on_open);
    connect(buttons, &QDialogButtonBox::accepted, this, &DialogMbmapLibrary::on_open);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setBusy(true, tr("Loading the list of maps..."));
    auto reply = _network.get(QNetworkRequest(QUrl(LibraryUrl + "index.json")));
    connect(reply, &QNetworkReply::finished, this, &DialogMbmapLibrary::on_indexFinished);
}

///
/// \brief DialogMbmapLibrary::setBusy
/// \param busy
/// \param status
///
void DialogMbmapLibrary::setBusy(bool busy, const QString& status)
{
    _list->setEnabled(!busy);
    _openButton->setEnabled(!busy && !_list->selectedItems().isEmpty());
    _status->setText(status);
}

///
/// \brief DialogMbmapLibrary::on_indexFinished
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
    _list->setSortingEnabled(false);
    for(auto&& value : maps)
    {
        const auto entry = value.toObject();
        const auto path = entry.value("path").toString();
        if(!MbMap::isMapFile(path)) continue;

        auto item = new QTreeWidgetItem(_list);
        item->setText(0, entry.value("vendor").toString());
        item->setText(1, entry.value("model").toString());
        item->setText(2, entry.value("description").toString());
        item->setData(3, Qt::DisplayRole, entry.value("registers").toInt());
        item->setData(0, PathRole, path);
    }
    _list->setSortingEnabled(true);
    _list->resizeColumnToContents(0);
    _list->resizeColumnToContents(1);

    setBusy(false, tr("%n map(s)", nullptr, _list->topLevelItemCount()));
}

///
/// \brief DialogMbmapLibrary::on_filterChanged
/// \param text
///
void DialogMbmapLibrary::on_filterChanged(const QString& text)
{
    const auto words = text.split(' ', Qt::SkipEmptyParts);
    for(int i = 0; i < _list->topLevelItemCount(); i++)
    {
        auto item = _list->topLevelItem(i);
        const auto haystack = QString("%1 %2 %3").arg(item->text(0), item->text(1), item->text(2));
        bool match = true;
        for(auto&& w : words)
            match = match && haystack.contains(w, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

///
/// \brief DialogMbmapLibrary::on_open
///
void DialogMbmapLibrary::on_open()
{
    const auto selected = _list->selectedItems();
    if(selected.isEmpty()) return;

    const auto path = selected.first()->data(0, PathRole).toString();
    setBusy(true, tr("Downloading %1...").arg(QFileInfo(path).fileName()));

    auto reply = _network.get(QNetworkRequest(QUrl(LibraryUrl + path)));
    reply->setProperty("path", path);
    connect(reply, &QNetworkReply::finished, this, &DialogMbmapLibrary::on_mapFinished);
}

///
/// \brief DialogMbmapLibrary::on_mapFinished
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
    const QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mbmap-library");
    const auto filename = dir.filePath(path);
    QDir().mkpath(QFileInfo(filename).absolutePath());

    QFile file(filename);
    if(!file.open(QFile::WriteOnly) || file.write(data) != data.size()) {
        setBusy(false, tr("The map could not be saved: %1").arg(file.errorString()));
        return;
    }

    _filename = filename;
    accept();
}
