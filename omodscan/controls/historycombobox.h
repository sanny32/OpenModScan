#ifndef HISTORYCOMBOBOX_H
#define HISTORYCOMBOBOX_H

#include <QComboBox>
#include <QSettings>
#include <QStringList>

///
/// \brief The HistoryComboBox class
///
class HistoryComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit HistoryComboBox(QWidget *parent = nullptr, int maxItems = 10, const QString &settingsKey = QString());
    ~HistoryComboBox();

    void addToHistory(const QString &text);
    void saveHistory();
    void loadHistory();
    void clearHistory();

    void setMaxItems(int max);
    int maxItems() const;

protected:
    void focusOutEvent(QFocusEvent *event) override;

private:
    int _maxItems;
    QString _settingsKey;
};

#endif // HISTORYCOMBOBOX_H
