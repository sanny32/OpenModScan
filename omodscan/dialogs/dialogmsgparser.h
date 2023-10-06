#ifndef DIALOGMSGPARSER_H
#define DIALOGMSGPARSER_H

#include <QDialog>

namespace Ui {
class DialogMsgParser;
}

class DialogMsgParser : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMsgParser(QWidget *parent = nullptr);
    ~DialogMsgParser();

    void accept() override;

private:
    Ui::DialogMsgParser *ui;
};

#endif // DIALOGMSGPARSER_H
