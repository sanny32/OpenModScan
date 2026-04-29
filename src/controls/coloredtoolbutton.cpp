#include "coloredtoolbutton.h"

#include <QApplication>
#include <QStyle>

///
/// \brief ColoredToolButton::ColoredToolButton
/// \param parent
///
ColoredToolButton::ColoredToolButton(QWidget* parent)
    : QToolButton(parent)
{
    clearColors();
}

///
/// \brief ColoredToolButton::setColors
/// \param colors
///
void ColoredToolButton::setColors(const Colors& colors)
{
    setStyleSheet(coloredStyleSheet(colors));
}

///
/// \brief ColoredToolButton::clearColors
///
void ColoredToolButton::clearColors()
{
    setStyleSheet(defaultStyleSheet());
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
/// \brief ColoredToolButton::background
/// \param top
/// \param bottom
/// \return
///
QString ColoredToolButton::background(const QString& top, const QString& bottom)
{
    return QString("qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2)")
        .arg(top, bottom);
}

///
/// \brief ColoredToolButton::defaultStyleSheet
/// \return
///
QString ColoredToolButton::defaultStyleSheet()
{
    return "padding: 0px 12px;";
}

///
/// \brief ColoredToolButton::coloredStyleSheet
/// \param colors
/// \return
///
QString ColoredToolButton::coloredStyleSheet(const Colors& colors)
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
                    ColoredToolButton {
                        color: white;
                        padding: 0px 12px 0px 2px;
                        background-color: %1;
                        border: 1px solid %2;
                        border-radius: 4px;
                    }
                    ColoredToolButton:hover {
                        background-color: %3;
                    }
                    ColoredToolButton:pressed {
                        background-color: %4;
                    }
                    ColoredToolButton::menu-button {
                        width: 12px;
                        border-left: 1px solid %2;
                    }
                    ColoredToolButton::menu-arrow {
                        width: 10px;
                        height: 10px;
                    }
                )").arg(normalBackground, colors.border, hoverBackground, pressedBackground);
}
