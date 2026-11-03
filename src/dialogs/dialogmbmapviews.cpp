#include <QHeaderView>
#include <QPushButton>
#include "dialogmbmapviews.h"
#include "ui_dialogmbmapviews.h"

namespace {

constexpr int IndexRole = Qt::UserRole + 1;

QString areaName(QModbusDataUnit::RegisterType area)
{
    switch(area)
    {
        case QModbusDataUnit::Coils:            return QObject::tr("Coils");
        case QModbusDataUnit::DiscreteInputs:   return QObject::tr("Discrete Inputs");
        case QModbusDataUnit::InputRegisters:   return QObject::tr("Input Registers");
        case QModbusDataUnit::HoldingRegisters: return QObject::tr("Holding Registers");
        default: return QString();
    }
}

}

///
/// \brief Lists the blocks of a map, each with a check box
/// \param map The map the blocks come from, for the title and the address base
/// \param views The blocks, as MbMap::views() lays them out
/// \param preselected How many blocks, from the first, are checked at first
/// \param parent Parent widget
///
DialogMbmapViews::DialogMbmapViews(const MbMap& map, const QList<MbMap::View>& views, int preselected, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogMbmapViews)
    ,_views(views)
{
    ui->setupUi(this);
    setWindowTitle(map.title());

    ui->labelIntro->setText(tr("This map has %n block(s) of registers. Each block opens in its own window: "
                               "choose the ones to open.", nullptr, views.size()));
    ui->treeWidget->header()->setSectionResizeMode(3, QHeaderView::Stretch);

    // Filled before the signals matter: itemChanged updates the Open button
    const QSignalBlocker blocker(ui->treeWidget);
    for(int i = 0; i < views.size(); i++)
    {
        const auto& v = views[i];
        const int first = v.Address + map.AddressBase;
        auto item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, v.Title);
        item->setText(1, areaName(v.Area));
        item->setText(2, v.Length > 1 ? QString("%1-%2").arg(first).arg(first + v.Length - 1) : QString::number(first));
        item->setText(3, QStringList(v.Descriptions.values()).join("; "));
        item->setToolTip(3, QStringList(v.Descriptions.values()).join("\n"));
        item->setData(0, IndexRole, i);
        item->setCheckState(0, i < preselected ? Qt::Checked : Qt::Unchecked);
    }
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);

    _openButton = ui->buttonBox->addButton(tr("Open"), QDialogButtonBox::AcceptRole);
    updateOpenButton();
}

///
/// \brief Deletes the dialog's form
///
DialogMbmapViews::~DialogMbmapViews()
{
    delete ui;
}

///
/// \brief The blocks to open
/// \return The checked blocks, in the order of the map
///
QList<MbMap::View> DialogMbmapViews::selectedViews() const
{
    QList<MbMap::View> selected;
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        const auto item = ui->treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked)
            selected.append(_views.at(item->data(0, IndexRole).toInt()));
    }
    return selected;
}

///
/// \brief Shows only the blocks whose title or registers contain the text
/// \param text The filter, matched without case
///
void DialogMbmapViews::on_lineEditFilter_textChanged(const QString& text)
{
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        auto item = ui->treeWidget->topLevelItem(i);
        item->setHidden(!item->text(0).contains(text, Qt::CaseInsensitive)
                        && !item->text(3).contains(text, Qt::CaseInsensitive));
    }
}

///
/// \brief Keeps the Open button in step with the checked blocks
/// \param item The block that changed
/// \param column The column that changed
///
void DialogMbmapViews::on_treeWidget_itemChanged(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(item)
    Q_UNUSED(column)
    updateOpenButton();
}

///
/// \brief Checks every block the filter shows
///
void DialogMbmapViews::on_pushButtonCheckShown_clicked()
{
    checkVisible(true);
}

///
/// \brief Unchecks every block the filter shows
///
void DialogMbmapViews::on_pushButtonUncheckShown_clicked()
{
    checkVisible(false);
}

///
/// \brief Checks or unchecks the blocks the filter shows
/// \param on true to check them, false to uncheck them
///
void DialogMbmapViews::checkVisible(bool on)
{
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        auto item = ui->treeWidget->topLevelItem(i);
        if(!item->isHidden())
            item->setCheckState(0, on ? Qt::Checked : Qt::Unchecked);
    }
}

///
/// \brief Enables the Open button when a block is checked, and counts the windows
///
void DialogMbmapViews::updateOpenButton()
{
    if(!_openButton) return;
    const int count = selectedViews().size();
    _openButton->setEnabled(count > 0);
    _openButton->setText(count > 1 ? tr("Open %1 Windows").arg(count) : tr("Open"));
}
