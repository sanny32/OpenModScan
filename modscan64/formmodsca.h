#ifndef FORMMODSCA_H
#define FORMMODSCA_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class FormModSca;
}

class FormModSca : public QWidget
{
    Q_OBJECT

public:
    explicit FormModSca(QWidget *parent = nullptr);
    ~FormModSca();

private slots:
    void updateListWidget();
    void on_comboBoxModbusPointType_currentTextChanged(const QString&);

private:
    Ui::FormModSca *ui;

private:
    QStandardItemModel _model;
};

#endif // FORMMODSCA_H
