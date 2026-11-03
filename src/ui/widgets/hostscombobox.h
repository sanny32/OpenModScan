#ifndef HOSTSCOMBOBOX_H
#define HOSTSCOMBOBOX_H

#include "historycombobox.h"

///
/// \brief The HostsComboBox class
///
class HostsComboBox : public HistoryComboBox
{
public:
    explicit HostsComboBox(QWidget *parent = nullptr, int maxItems = 10);
    ~HostsComboBox();

    void addItem(const QString &text, const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);

    void saveHistory();

protected:
    void focusOutEvent(QFocusEvent *event);

private slots:
    void updateCompleter();
};

#endif // HOSTSCOMBOBOX_H
