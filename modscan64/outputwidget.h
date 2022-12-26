#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>
#include "enums.h"
#include "displaydefinition.h"

namespace Ui {
class OutputWidget;
}

///
/// \brief The OutputWidget class
///
class OutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(QWidget *parent = nullptr);
    ~OutputWidget();

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    DataDisplayMode dataDisplayMode() const;
    void setDataDisplayMode(DataDisplayMode mode);

    void update(const DisplayDefinition& dd);

private:
    void updateDataWidget();
    void updateTrafficWidget();

private:
    Ui::OutputWidget *ui;

private:
    DisplayMode _displayMode;
    DataDisplayMode _dataDisplayMode;
    DisplayDefinition _displayDefinition;
};

#endif // OUTPUTWIDGET_H
