#ifndef ABOUTDATAWIDGET_H
#define ABOUTDATAWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QUrl>

///
/// \brief The AboutDataWidget class
///
class AboutDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AboutDataWidget(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setVersion(const QString &version);
    void setDescription(const QString &description);
    void setLinkUrl(const QUrl &url);
    void setLinkIcon(const QIcon &icon);
    void setLinkToolTip(const QString &toolTip);

    QString title() const;
    QString version() const;
    QString description() const;
    QUrl linkUrl() const;
    QString linkToolTip() const;

private slots:
    void on_linkClicked();

private:
    void setupUI();
    void updateLinkButton();

private:
    QLabel* _titleLabel;
    QLabel* _versionLabel;
    QLabel* _descriptionLabel;
    QFrame* _underline;
    QPushButton* _linkButton;
    QUrl _linkUrl;
};

#endif // ABOUTDATAWIDGET_H
