#ifndef TRANSPSCROLLAREA_H
#define TRANSPSCROLLAREA_H

#include <QObject>
#include <QScrollArea>

///
/// \brief The Transparent ScrollArea class
///
class TranspScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    TranspScrollArea(QWidget* parent = nullptr);
};

#endif // TRANSPSCROLLAREA_H
