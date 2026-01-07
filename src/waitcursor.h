#ifndef WAITCURSOR_H
#define WAITCURSOR_H

#include <QWidget>
#include <QApplication>

class WaitCursor
{
public:
    explicit WaitCursor(QWidget* widget = nullptr)
    {
        _widget = widget;

        if(_widget != nullptr)
        {
            _widget->setCursor(Qt::WaitCursor);
        }
        else
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
        }

        QApplication::processEvents();
    }

    ~WaitCursor()
    {
        if(_widget != nullptr)
        {
            _widget->unsetCursor();
        }
        else
        {
            QApplication::restoreOverrideCursor();
        }

        QApplication::processEvents();
    }

private:
    QWidget* _widget;
};

#endif // WAITCURSOR_H
