#ifndef COLOREDPUSHBUTTON_H
#define COLOREDPUSHBUTTON_H

#include <QPushButton>
#include <QString>

///
/// \brief The ColoredPushButton class
///
class ColoredPushButton : public QPushButton
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

    explicit ColoredPushButton(QWidget* parent = nullptr);

    void setColors(const Colors& colors);
    void clearColors();

private:
    static bool isFusionStyle();
    static QString background(const QString& top, const QString& bottom);
    static QString defaultStyleSheet();
    static QString coloredStyleSheet(const Colors& colors);
};

#endif // COLOREDPUSHBUTTON_H
