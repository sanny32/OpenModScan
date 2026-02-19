#include <QCompleter>
#include <QHostAddress>
#include "hostscombobox.h"

///
/// \brief HostsComboBox::HostsComboBox
/// \param parent
/// \param maxItems
///
HostsComboBox::HostsComboBox(QWidget *parent, int maxItems)
    :HistoryComboBox(parent, maxItems, "hosts")
{
    QCompleter *completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(completer);

    updateCompleter();
}

///
/// \brief HostsComboBox::~HostsComboBox
///
HostsComboBox::~HostsComboBox()
{
}

///
/// \brief HostsComboBox::addItem
/// \param text
/// \param userData
///
void HostsComboBox::addItem(const QString &text, const QVariant &userData)
{
    // Remove duplicates before adding
    int index = findText(text);
    if (index >= 0)
        removeItem(index);

    HistoryComboBox::addItem(text, userData);
}

///
/// \brief HostsComboBox::addItems
/// \param texts
///
void HostsComboBox::addItems(const QStringList &texts)
{
    for (const QString &text : texts)
        addItem(text);
}

///
/// \brief HostsComboBox::saveHistory
///
void HostsComboBox::saveHistory()
{
    addItem(currentText());
    HistoryComboBox::saveHistory();
}

///
/// \brief HostsComboBox::updateCompleter
///
void HostsComboBox::updateCompleter()
{
    completer()->setModel(model());
}

///
/// \brief HostsComboBox::focusOutEvent
/// \param event
///
void HostsComboBox::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
}
