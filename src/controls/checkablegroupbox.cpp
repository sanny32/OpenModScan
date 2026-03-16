#include "checkablegroupbox.h"
#include <QStylePainter>
#include <QStyleOptionGroupBox>

///
/// \brief CheckableGroupBox::CheckableGroupBox
/// \param parent
///
CheckableGroupBox::CheckableGroupBox(QWidget* parent)
    : QGroupBox(parent)
{
    setCheckable(true);
    setChecked(true);

    // Qt disables child widgets when the group box is unchecked.
    // Re-enable them immediately so they always remain interactive.
    connect(this, &QGroupBox::toggled, this, [this](bool checked) {
        if (!checked) {
            for (QWidget* child : findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
                child->setEnabled(true);
        }
    });

    connect(this, &QGroupBox::clicked, this, &CheckableGroupBox::onClicked);
}

///
/// \brief CheckableGroupBox::setCheckState
/// \param state
///
void CheckableGroupBox::setCheckState(Qt::CheckState state)
{
    if (_checkState == state)
        return;

    _checkState = state;
    update();
}

///
/// \brief CheckableGroupBox::onClicked
///
void CheckableGroupBox::onClicked()
{
    // Qt toggled internal checked to false; restore it so children stay enabled.
    blockSignals(true);
    setChecked(true);
    blockSignals(false);

    // Cycle: Checked → Unchecked, anything else (Partial/Unchecked) → Checked
    const Qt::CheckState newState = (_checkState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    _checkState = newState;
    update();

    emit checkStateChanged(newState);
}

///
/// \brief CheckableGroupBox::paintEvent
/// \param event
///
void CheckableGroupBox::paintEvent(QPaintEvent*)
{
    QStylePainter paint(this);
    QStyleOptionGroupBox option;
    initStyleOption(&option);

    // Override the checkbox state flags with our tristate value
    option.state &= ~(QStyle::State_On | QStyle::State_Off | QStyle::State_NoChange);
    switch (_checkState) {
        case Qt::Checked:          option.state |= QStyle::State_On;        break;
        case Qt::Unchecked:        option.state |= QStyle::State_Off;       break;
        case Qt::PartiallyChecked: option.state |= QStyle::State_NoChange;  break;
    }

    paint.drawComplexControl(QStyle::CC_GroupBox, option);
}
