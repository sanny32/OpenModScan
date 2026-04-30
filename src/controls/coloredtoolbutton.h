#ifndef COLOREDTOOLBUTTON_H
#define COLOREDTOOLBUTTON_H

#include <QToolButton>
#include <QColor>
#include <QString>

class QBrush;
class QPaintEvent;
class QPalette;
class QPainter;
class QStyleOptionToolButton;

///
/// \brief The ColoredToolButton class
///
class ColoredToolButton : public QToolButton
{
    Q_OBJECT

public:
    struct Colors
    {
        QString base;
        QString hover;
        QString pressed;
        QString border;
    };

    explicit ColoredToolButton(QWidget* parent = nullptr);

    void setColors(const Colors& colors);
    void clearColors();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static bool isFusionStyle();

    QColor buttonColor(const QStyleOptionToolButton& option) const;
    QBrush buttonBrush(const QStyleOptionToolButton& option) const;
    QPalette coloredPalette(const QStyleOptionToolButton& option) const;
    void drawColoredToolButton(QPainter* painter, const QStyleOptionToolButton& option) const;

    Colors _colors;
    bool _hasColors = false;
};

#endif // COLOREDTOOLBUTTON_H
