#include "historycombobox.h"

///
/// \brief HistoryComboBox::HistoryComboBox
/// \param parent
/// \param maxItems
/// \param settingsKey
///
HistoryComboBox::HistoryComboBox(QWidget *parent, int maxItems, const QString &settingsKey)
    : QComboBox(parent), _maxItems(maxItems), _settingsKey(settingsKey)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    loadHistory();
}

///
/// \brief HistoryComboBox::~HistoryComboBox
///
HistoryComboBox::~HistoryComboBox()
{
}

///
/// \brief HistoryComboBox::addToHistory
/// \param text
///
void HistoryComboBox::addToHistory(const QString &text)
{
    if (text.isEmpty())
        return;

    // Remove duplicates
    int index = findText(text);
    if (index >= 0)
        removeItem(index);

    // Add to the beginning
    insertItem(0, text);

    // Limit the number of items
    while (count() > _maxItems)
        removeItem(_maxItems);
}

///
/// \brief HistoryComboBox::saveHistory
///
void HistoryComboBox::saveHistory()
{
    if (_settingsKey.isEmpty())
        return;

    QSettings m(QSettings::NativeFormat, QSettings::UserScope, APP_NAME, APP_VERSION);
    m.beginGroup("HistoryComboBox");

    QStringList history;
    for (int i = 0; i < count(); ++i)
        history << itemText(i);

    m.setValue(_settingsKey, history);
    m.endGroup();
}

///
/// \brief HistoryComboBox::loadHistory
///
void HistoryComboBox::loadHistory()
{
    if (_settingsKey.isEmpty())
        return;

    QSettings m(QSettings::NativeFormat, QSettings::UserScope, APP_NAME, APP_VERSION);
    m.beginGroup("HistoryComboBox");
    QStringList history = m.value(_settingsKey).toStringList();

    if (!history.isEmpty())
        addItems(history);

    m.endGroup();
}

///
/// \brief HistoryComboBox::clearHistory
///
void HistoryComboBox::clearHistory()
{
    clear();
    if (!_settingsKey.isEmpty())
    {
        QSettings m(QSettings::NativeFormat, QSettings::UserScope, APP_NAME, APP_VERSION);
        m.beginGroup("HistoryComboBox");
        m.remove(_settingsKey);
        m.endGroup();
    }
}

///
/// \brief HistoryComboBox::setMaxItems
/// \param max
///
void HistoryComboBox::setMaxItems(int max)
{
    _maxItems = max;
    while (count() > _maxItems)
        removeItem(_maxItems);
}

///
/// \brief HistoryComboBox::maxItems
/// \return
///
int HistoryComboBox::maxItems() const
{
    return _maxItems;
}

///
/// \brief HistoryComboBox::focusOutEvent
/// \param event
///
void HistoryComboBox::focusOutEvent(QFocusEvent *event)
{
    QString currentText = this->currentText();
    if (!currentText.isEmpty() && findText(currentText) == -1)
        addToHistory(currentText);

    QComboBox::focusOutEvent(event);
}
