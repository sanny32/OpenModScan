#ifndef BYTELISTTEXTEDIT_H
#define BYTELISTTEXTEDIT_H

#include <QValidator>
#include <QPlainTextEdit>

///
/// \brief The ByteListTextEdit class
///
class ByteListTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    enum InputMode
    {
        DecMode = 0,
        HexMode
    };

    explicit ByteListTextEdit(QWidget* parent = nullptr);
    explicit ByteListTextEdit(InputMode mode, QWidget *parent = nullptr);

    QByteArray value() const;
    void setValue(const QByteArray& value);

    InputMode inputMode() const;
    void setInputMode(InputMode mode);

    QString text() const;
    void setText(const QString& text);

    bool isEmpty() const {
        return text().isEmpty();
    }

signals:
    void valueChanged(const QByteArray& value);

protected:
    void focusOutEvent(QFocusEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    bool canInsertFromMimeData(const QMimeData* source) const override;
    void insertFromMimeData(const QMimeData* source) override;

private slots:
    void on_textChanged();

private:
    void updateValue();

private:
    InputMode _inputMode;
    QByteArray _value;
    QChar _separator;
    QValidator* _validator;
};

#endif // BYTELISTTEXTEDIT_H
