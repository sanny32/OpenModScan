#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include "dialogmbmapviews.h"

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
/// \brief DialogMbmapViews::DialogMbmapViews
/// \param map
/// \param views
/// \param preselected How many blocks are checked at first
/// \param parent
///
DialogMbmapViews::DialogMbmapViews(const MbMap& map, const QList<MbMap::View>& views, int preselected, QWidget* parent)
    : QDialog(parent)
    ,_views(views)
    ,_filter(new QLineEdit(this))
    ,_list(new QTreeWidget(this))
{
    setWindowTitle(map.title());
    resize(760, 520);

    auto intro = new QLabel(tr("This map has %n block(s) of registers. Each block opens in its own window: "
                               "choose the ones to open.", nullptr, views.size()), this);
    intro->setWordWrap(true);

    _filter->setPlaceholderText(tr("Filter by register name"));
    _filter->setClearButtonEnabled(true);

    _list->setRootIsDecorated(false);
    _list->setAlternatingRowColors(true);
    _list->setHeaderLabels({ tr("Window"), tr("Area"), tr("Addresses"), tr("Registers") });
    _list->header()->setSectionResizeMode(3, QHeaderView::Stretch);

    for(int i = 0; i < views.size(); i++)
    {
        const auto& v = views[i];
        const int first = v.Address + map.AddressBase;
        auto item = new QTreeWidgetItem(_list);
        item->setText(0, v.Title);
        item->setText(1, areaName(v.Area));
        item->setText(2, v.Length > 1 ? QString("%1-%2").arg(first).arg(first + v.Length - 1) : QString::number(first));
        item->setText(3, QStringList(v.Descriptions.values()).join("; "));
        item->setToolTip(3, QStringList(v.Descriptions.values()).join("\n"));
        item->setData(0, IndexRole, i);
        item->setCheckState(0, i < preselected ? Qt::Checked : Qt::Unchecked);
    }
    _list->resizeColumnToContents(0);
    _list->resizeColumnToContents(1);
    _list->resizeColumnToContents(2);

    auto selectAll = new QPushButton(tr("Check shown"), this);
    auto selectNone = new QPushButton(tr("Uncheck shown"), this);
    connect(selectAll, &QPushButton::clicked, this, [this] { checkVisible(true); });
    connect(selectNone, &QPushButton::clicked, this, [this] { checkVisible(false); });

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    _openButton = buttons->addButton(tr("Open"), QDialogButtonBox::AcceptRole);

    auto bottom = new QHBoxLayout;
    bottom->addWidget(selectAll);
    bottom->addWidget(selectNone);
    bottom->addStretch();
    bottom->addWidget(buttons);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(intro);
    layout->addWidget(_filter);
    layout->addWidget(_list);
    layout->addLayout(bottom);

    connect(_filter, &QLineEdit::textChanged, this, &DialogMbmapViews::on_filterChanged);
    connect(_list, &QTreeWidget::itemChanged, this, &DialogMbmapViews::updateOpenButton);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    updateOpenButton();
}

///
/// \brief DialogMbmapViews::selectedViews
/// \return
///
QList<MbMap::View> DialogMbmapViews::selectedViews() const
{
    QList<MbMap::View> selected;
    for(int i = 0; i < _list->topLevelItemCount(); i++) {
        const auto item = _list->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked)
            selected.append(_views.at(item->data(0, IndexRole).toInt()));
    }
    return selected;
}

///
/// \brief DialogMbmapViews::on_filterChanged
/// \param text
///
void DialogMbmapViews::on_filterChanged(const QString& text)
{
    for(int i = 0; i < _list->topLevelItemCount(); i++) {
        auto item = _list->topLevelItem(i);
        item->setHidden(!item->text(0).contains(text, Qt::CaseInsensitive)
                        && !item->text(3).contains(text, Qt::CaseInsensitive));
    }
}

///
/// \brief DialogMbmapViews::checkVisible
/// \param on
///
void DialogMbmapViews::checkVisible(bool on)
{
    for(int i = 0; i < _list->topLevelItemCount(); i++) {
        auto item = _list->topLevelItem(i);
        if(!item->isHidden())
            item->setCheckState(0, on ? Qt::Checked : Qt::Unchecked);
    }
}

///
/// \brief DialogMbmapViews::updateOpenButton
///
void DialogMbmapViews::updateOpenButton()
{
    const int count = selectedViews().size();
    _openButton->setEnabled(count > 0);
    _openButton->setText(count > 1 ? tr("Open %1 Windows").arg(count) : tr("Open"));
}
