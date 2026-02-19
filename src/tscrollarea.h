#ifndef TSCROLLAREA_H
#define TSCROLLAREA_H

#include <QObject>
#include <QScrollArea>

///
/// \brief The Transparent ScrollArea class
///
class TScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    TScrollArea(QWidget* parent = nullptr);
};

#endif // TSCROLLAREA_H
