#ifndef HEXVIEWBUTTON_H
#define HEXVIEWBUTTON_H

#include <QToolButton>

///
/// \brief The HexViewButton class
///
class HexViewButton : public QToolButton
{
    Q_OBJECT
public:
    explicit HexViewButton(QWidget* parent = nullptr);
};

#endif // HEXVIEWBUTTON_H
