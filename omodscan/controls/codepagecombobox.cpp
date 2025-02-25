#include <QEvent>
#include <QTextCodec>
#include "codepagecombobox.h"

///
/// \brief CodepageComboBox::CodepageComboBox
///
CodepageComboBox::CodepageComboBox(QWidget* parent)
    : QComboBox(parent)
{
    const QStringList encodings =
    {
        "Windows-1250",
        "Windows-1251",
        "Windows-1252",
        "Windows-1253",
        "Windows-1254",
        "Windows-1255",
        "Windows-1256",
        "Windows-1257",
        "Windows-1258",
        "KOI8-R",
        "KOI8-U",
        "ISO 8859-1",
        "ISO 8859-2",
        "ISO 8859-3",
        "ISO 8859-4",
        "ISO 8859-5",
        "ISO 8859-6",
        "ISO 8859-7",
        "ISO 8859-8",
        "ISO 8859-9",
        "ISO 8859-10",
        "ISO 8859-13",
        "ISO 8859-14",
        "ISO 8859-15",
        "ISO 8859-16",
    };
    addItems(encodings);

    setCurrentIndex(0);

    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CodepageComboBox::on_currentIndexChanged);
}

///
/// \brief CodepageComboBox::changeEvent
/// \param event
///
void CodepageComboBox::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
    }

    QComboBox::changeEvent(event);
}

///
/// \brief CodepageComboBox::currentCodepage
/// \return
///
QString CodepageComboBox::currentCodepage() const
{
    return currentText();
}

///
/// \brief CodepageComboBox::setCurrentCodepage
/// \param codepage
///
void CodepageComboBox::setCurrentCodepage(const QString& codepage)
{
    const auto idx = findText(codepage);
    setCurrentIndex(idx);
}

///
/// \brief CodepageComboBox::on_currentIndexChanged
/// \param index
///
void CodepageComboBox::on_currentIndexChanged(int index)
{
    emit codepageChanged(itemData(index, Qt::DisplayRole).toString());
}
