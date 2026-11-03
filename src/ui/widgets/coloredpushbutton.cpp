#include "coloredpushbutton.h"

#include <QApplication>
#include <QStyle>

///
/// \brief ColoredPushButton::ColoredPushButton
/// \param parent
///
ColoredPushButton::ColoredPushButton(QWidget* parent)
    : QPushButton(parent)
{
    clearColors();
}

///
/// \brief ColoredPushButton::setColors
/// \param colors
///
void ColoredPushButton::setColors(const Colors& colors)
{
    setStyleSheet(coloredStyleSheet(colors));
}

///
/// \brief ColoredPushButton::clearColors
///
void ColoredPushButton::clearColors()
{
    setStyleSheet(defaultStyleSheet());
}

///
/// \brief ColoredPushButton::isFusionStyle
/// \return
///
bool ColoredPushButton::isFusionStyle()
{
    return qApp != nullptr
        && qApp->style() != nullptr
        && qApp->style()->objectName().compare("fusion", Qt::CaseInsensitive) == 0;
}

///
/// \brief ColoredPushButton::background
/// \param top
/// \param bottom
/// \return
///
QString ColoredPushButton::background(const QString& top, const QString& bottom)
{
    return QString("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2)")
        .arg(top, bottom);
}

///
/// \brief ColoredPushButton::defaultStyleSheet
/// \return
///
QString ColoredPushButton::defaultStyleSheet()
{
    return "padding: 4px 12px;";
}

///
/// \brief ColoredPushButton::coloredStyleSheet
/// \param colors
/// \return
///
QString ColoredPushButton::coloredStyleSheet(const Colors& colors)
{
    const QString normalBackground = isFusionStyle()
        ? background(colors.base, colors.hover)
        : colors.base;
    const QString hoverBackground = isFusionStyle()
        ? background(colors.hover, colors.pressed)
        : colors.hover;
    const QString pressedBackground = isFusionStyle()
        ? background(colors.pressed, colors.border)
        : colors.pressed;

    return QString(R"(
                    ColoredPushButton {
                        color: white;
                        padding: 4px 12px;
                        background-color: %1;
                        border: 1px solid %2;
                        border-radius: 4px;
                    }
                    ColoredPushButton:hover {
                        background-color: %3;
                    }
                    ColoredPushButton:pressed {
                        background-color: %4;
                    }
                )").arg(normalBackground, colors.border, hoverBackground, pressedBackground);
}
