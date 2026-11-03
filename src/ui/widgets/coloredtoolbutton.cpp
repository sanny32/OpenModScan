#include "coloredtoolbutton.h"

#include <QApplication>
#include <QBrush>
#include <QLinearGradient>
#include <QPainter>
#include <QPalette>
#include <QStyle>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>

///
/// \brief ColoredToolButton::ColoredToolButton
/// \param parent
///
ColoredToolButton::ColoredToolButton(QWidget* parent)
    : QToolButton(parent)
{
}

///
/// \brief ColoredToolButton::setColors
/// \param colors
///
void ColoredToolButton::setColors(const Colors& colors)
{
    _colors = colors;
    _hasColors = true;
    update();
}

///
/// \brief ColoredToolButton::clearColors
///
void ColoredToolButton::clearColors()
{
    _hasColors = false;
    update();
}

///
/// \brief ColoredToolButton::paintEvent
/// \param event
///
void ColoredToolButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QStyleOptionToolButton option;
    initStyleOption(&option);

    if(!_hasColors) {
        QStylePainter painter(this);
        painter.drawComplexControl(QStyle::CC_ToolButton, option);
        return;
    }

    QStylePainter painter(this);
    option.palette = coloredPalette(option);
    drawColoredToolButton(&painter, option);
}

///
/// \brief ColoredToolButton::isFusionStyle
/// \return
///
bool ColoredToolButton::isFusionStyle()
{
    return qApp != nullptr
        && qApp->style() != nullptr
        && qApp->style()->objectName().compare("fusion", Qt::CaseInsensitive) == 0;
}

///
/// \brief ColoredToolButton::buttonColor
/// \param option
/// \return
///
QColor ColoredToolButton::buttonColor(const QStyleOptionToolButton& option) const
{
    if(option.state & (QStyle::State_Sunken | QStyle::State_On))
        return QColor(_colors.pressed);

    if(option.state & QStyle::State_MouseOver)
        return QColor(_colors.hover);

    return QColor(_colors.base);
}

///
/// \brief ColoredToolButton::buttonBrush
/// \param option
/// \return
///
QBrush ColoredToolButton::buttonBrush(const QStyleOptionToolButton& option) const
{
    QColor top;
    QColor bottom;

    if(option.state & (QStyle::State_Sunken | QStyle::State_On)) {
        top = QColor(_colors.pressed);
        bottom = QColor(_colors.border);
    }
    else if(option.state & QStyle::State_MouseOver) {
        top = QColor(_colors.hover);
        bottom = QColor(_colors.pressed);
    }
    else {
        top = QColor(_colors.base);
        bottom = QColor(_colors.hover);
    }

    if(!isFusionStyle())
        return top;

    QLinearGradient gradient(option.rect.topLeft(), option.rect.bottomLeft());
    gradient.setColorAt(0.0, top);
    gradient.setColorAt(1.0, bottom);
    return gradient;
}

///
/// \brief ColoredToolButton::coloredPalette
/// \param option
/// \return
///
QPalette ColoredToolButton::coloredPalette(const QStyleOptionToolButton& option) const
{
    const QColor button = buttonColor(option);
    const QColor border(_colors.border);
    const QColor text(Qt::white);

    QPalette palette = option.palette;

    for(const QPalette::ColorGroup group : { QPalette::Active, QPalette::Inactive, QPalette::Disabled }) {
        palette.setColor(group, QPalette::Button, button);
        palette.setColor(group, QPalette::ButtonText, text);
        palette.setColor(group, QPalette::WindowText, text);
        palette.setColor(group, QPalette::Light, QColor(_colors.hover).lighter(115));
        palette.setColor(group, QPalette::Midlight, QColor(_colors.hover));
        palette.setColor(group, QPalette::Mid, border);
        palette.setColor(group, QPalette::Dark, border);
        palette.setColor(group, QPalette::Shadow, border.darker(125));
    }

    return palette;
}

///
/// \brief ColoredToolButton::drawColoredToolButton
/// \param painter
/// \param option
///
void ColoredToolButton::drawColoredToolButton(QPainter* painter, const QStyleOptionToolButton& option) const
{
    const QRect buttonRect = style()->subControlRect(QStyle::CC_ToolButton, &option, QStyle::SC_ToolButton, this);
    const QRect menuRect = style()->subControlRect(QStyle::CC_ToolButton, &option, QStyle::SC_ToolButtonMenu, this);
    const QColor border(_colors.border);

    QStyle::State buttonState = option.state & ~QStyle::State_Sunken;
    QStyle::State menuState = buttonState;
    const bool autoRaise = option.state & QStyle::State_AutoRaise;

    if(autoRaise && (!(buttonState & QStyle::State_MouseOver) || !(buttonState & QStyle::State_Enabled)))
        buttonState &= ~QStyle::State_Raised;

    if(option.state & QStyle::State_Sunken) {
        if(option.activeSubControls & QStyle::SC_ToolButton) {
            buttonState |= QStyle::State_Sunken;
            menuState |= QStyle::State_MouseOver | QStyle::State_Sunken;
        }
        else if(option.activeSubControls & QStyle::SC_ToolButtonMenu) {
            menuState |= QStyle::State_Sunken;
            buttonState |= QStyle::State_MouseOver;
        }
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(border);
    painter->setBrush(buttonBrush(option));
    painter->drawRoundedRect(option.rect.adjusted(0, 0, -1, -1), 4.0, 4.0);
    painter->restore();

    if(option.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect focus;
        focus.QStyleOption::operator=(option);
        focus.rect.adjust(3, 3, -3, -3);
        if(option.features & QStyleOptionToolButton::MenuButtonPopup)
            focus.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &option, this), 0);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &focus, painter, this);
    }

    QStyleOptionToolButton label = option;
    label.state = buttonState;
    if(!autoRaise)
        label.state &= ~QStyle::State_Sunken;
    label.rect = buttonRect;
    if(option.subControls & QStyle::SC_ToolButtonMenu) {
        if(isRightToLeft())
            label.rect.setLeft(menuRect.right() + 1);
        else
            label.rect.setRight(menuRect.left() - 1);
    }
    label.rect.adjust(2, 2, -2, -2);
    style()->drawControl(QStyle::CE_ToolButtonLabel, &label, painter, this);

    if(option.subControls & QStyle::SC_ToolButtonMenu) {
        QStyleOptionToolButton tool = option;
        tool.state = menuState;

        painter->save();
        painter->setPen(option.palette.dark().color());
        painter->drawLine(menuRect.left(), menuRect.top() + 3,
                          menuRect.left(), menuRect.bottom() - 3);
        painter->setPen(option.palette.light().color());
        painter->drawLine(menuRect.left() - 1, menuRect.top() + 3,
                          menuRect.left() - 1, menuRect.bottom() - 3);

        tool.rect = menuRect.adjusted(2, 3, -2, -1);
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, painter, this);
        painter->restore();
    }
    else if(option.features & QStyleOptionToolButton::HasMenu) {
        const int indicator = style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &option, this);
        QStyleOptionToolButton arrow = option;
        arrow.rect = QRect(option.rect.right() + 4 - indicator,
                           option.rect.height() - indicator + 4,
                           indicator - 5,
                           indicator - 5);
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrow, painter, this);
    }
}
