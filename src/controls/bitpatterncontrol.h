#ifndef BITPATTERNCONTROL_H
#define BITPATTERNCONTROL_H

#include <QWidget>

namespace Ui {
class BitPatternControl;
}

class BitPatternControl : public QWidget
{
    Q_OBJECT

public:
    explicit BitPatternControl(QWidget *parent = nullptr);
    ~BitPatternControl();

    quint16 value() const;
    void setValue(quint16 value);

signals:
    void valueChanged(quint16 value);

private slots:
    void on_checkStateChanged(int);

private:
    Ui::BitPatternControl *ui;
};

#endif // BITPATTERNCONTROL_H
