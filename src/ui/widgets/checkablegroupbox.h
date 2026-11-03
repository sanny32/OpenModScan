#ifndef CHECKABLEGROUPBOX_H
#define CHECKABLEGROUPBOX_H

#include <QGroupBox>

class CheckableGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit CheckableGroupBox(QWidget* parent = nullptr);

    Qt::CheckState checkState() const { return _checkState; }
    void setCheckState(Qt::CheckState state);

signals:
    void checkStateChanged(Qt::CheckState state);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onClicked();

private:
    Qt::CheckState _checkState = Qt::Checked;
};

#endif // CHECKABLEGROUPBOX_H
