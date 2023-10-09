#ifndef DIALOGMSGPARSER_H
#define DIALOGMSGPARSER_H

#include <QDialog>
#include "enums.h"

namespace Ui {
class DialogMsgParser;
}

///
/// \brief The DialogMsgParser class
///
class DialogMsgParser : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMsgParser(DataDisplayMode mode, QWidget *parent = nullptr);
    ~DialogMsgParser();

    void accept() override;

private slots:
    void on_awake();
    void on_hexView_toggled(bool);

private:
    Ui::DialogMsgParser *ui;
};

#endif // DIALOGMSGPARSER_H
