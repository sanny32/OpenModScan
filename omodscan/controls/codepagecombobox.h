#ifndef CODEPAGECOMBOBOX_H
#define CODEPAGECOMBOBOX_H

#include <QComboBox>
#include <QObject>

///
/// \brief The CodepageComboBox class
///
class CodepageComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit CodepageComboBox(QWidget *parent = nullptr);

    QString currentCodepage() const;
    void setCurrentCodepage(const QString& codepage);

protected:
    void changeEvent(QEvent* event) override;

signals:
    void codepageChanged(const QString& codepage);

private slots:
    void on_currentIndexChanged(int);
};

#endif // CODEPAGECOMBOBOX_H
