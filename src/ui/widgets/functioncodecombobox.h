#ifndef FUNCTIONCODECOMBOBOX_H
#define FUNCTIONCODECOMBOBOX_H

#include <QComboBox>
#include <QModbusPdu>

class FunctionCodeComboBox : public QComboBox
{
    Q_OBJECT

public:
    enum InputMode
    {
        DecMode = 0,
        HexMode
    };

    explicit FunctionCodeComboBox(QWidget *parent = nullptr);

    QModbusPdu::FunctionCode currentFunctionCode() const;
    void setCurrentFunctionCode(QModbusPdu::FunctionCode funcCode);

    InputMode inputMode() const;
    void setInputMode(InputMode mode);

    void addItem(QModbusPdu::FunctionCode funcCode);
    void addItems(const QVector<QModbusPdu::FunctionCode>& funcCodes);

signals:
    void functionCodeChanged(QModbusPdu::FunctionCode funcCode);

protected:
    void focusInEvent(QFocusEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private slots:
    void on_currentIndexChanged(int);
    void on_currentTextChanged(const QString&);

private:
    void update();
    QString formatFuncCode(QModbusPdu::FunctionCode funcCode) const;

private:
    InputMode _inputMode;
    QModbusPdu::FunctionCode _currentFunc;
    QValidator* _validator;
};

#endif // FUNCTIONCODECOMBOBOX_H
