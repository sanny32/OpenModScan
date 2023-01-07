#ifndef BYTELISTLINEEDIT_H
#define BYTELISTLINEEDIT_H

#include <QLineEdit>

///
/// \brief The ByteListLineEdit class
///
class ByteListLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    enum InputMode
    {
        DecMode = 0,
        HexMode
    };

    explicit ByteListLineEdit(QWidget* parent = nullptr);
    explicit ByteListLineEdit(InputMode mode, QWidget *parent = nullptr);

    QByteArray value() const;
    void setValue(const QByteArray& value);

    InputMode inputMode() const;
    void setInputMode(InputMode mode);

    void setText(const QString& text);

signals:
    void valueChanged(const QByteArray& value);

protected:
    void focusOutEvent(QFocusEvent*) override;

private slots:
    void on_editingFinished();
    void on_textChanged(const QString& text);

private:
    void updateValue();

private:
    InputMode _inputMode;
    QByteArray _value;
};

#endif // BYTELISTLINEEDIT_H
