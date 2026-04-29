#ifndef COLOREDTOOLBUTTON_H
#define COLOREDTOOLBUTTON_H

#include <QToolButton>
#include <QString>

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

private:
    static bool isFusionStyle();
    static QString background(const QString& top, const QString& bottom);
    static QString defaultStyleSheet();
    static QString coloredStyleSheet(const Colors& colors);
};

#endif // COLOREDTOOLBUTTON_H
