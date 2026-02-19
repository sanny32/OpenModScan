#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include "aboutdatawidget.h"

///
/// \brief AboutDataWidget::AboutDataWidget
/// \param parent
///
AboutDataWidget::AboutDataWidget(QWidget *parent)
    : QWidget(parent)
    ,_titleLabel(new QLabel(this))
    ,_versionLabel(new QLabel(this))
    ,_descriptionLabel(new QLabel(this))
    ,_underline(new QFrame(this))
    ,_linkButton(new QPushButton(this))
    ,_linkUrl()
{
    setupUI();
}

///
/// \brief AboutDataWidget::setupUI
///
void AboutDataWidget::setupUI()
{
    auto mainLayout = new QVBoxLayout(this);
    auto infoLayout = new QHBoxLayout();
    auto textLayout = new QVBoxLayout();
    auto titleLayout = new QHBoxLayout();

    mainLayout->setSpacing(4);
    textLayout->setSpacing(2);
    titleLayout->setSpacing(4);

    QFont titleFont = _titleLabel->font();
    titleFont.setBold(true);
    _titleLabel->setFont(titleFont);

    QPalette versionPalette = _versionLabel->palette();
    versionPalette.setColor(QPalette::WindowText, Qt::darkGray);
    _versionLabel->setPalette(versionPalette);

    _linkButton->setCursor(Qt::PointingHandCursor);
    _linkButton->setFlat(true);
    _linkButton->setFixedSize(20, 20);
    _linkButton->setIconSize(QSize(16, 16));

    _underline->setFrameShape(QFrame::HLine);
    _underline->setFrameShadow(QFrame::Sunken);
    _underline->setLineWidth(1);
    _underline->setMidLineWidth(0);

    connect(_linkButton, &QPushButton::clicked, this, &AboutDataWidget::on_linkClicked);

    titleLayout->addWidget(_titleLabel);
    titleLayout->addWidget(_versionLabel);
    titleLayout->addStretch();

    textLayout->addLayout(titleLayout);
    textLayout->addWidget(_descriptionLabel);

    infoLayout->addLayout(textLayout);
    infoLayout->addWidget(_linkButton);

    _descriptionLabel->setWordWrap(true);

    mainLayout->addLayout(infoLayout);
    mainLayout->addWidget(_underline);

    _linkButton->setVisible(false);
}

///
/// \brief AboutDataWidget::setTitle
/// \param title
///
void AboutDataWidget::setTitle(const QString &title)
{
    _titleLabel->setText(title);
}

///
/// \brief AboutDataWidget::setVersion
/// \param version
///
void AboutDataWidget::setVersion(const QString &version)
{
    _versionLabel->setText(QString("(%1)").arg(version));
}

///
/// \brief AboutDataWidget::setDescription
/// \param description
///
void AboutDataWidget::setDescription(const QString &description)
{
    _descriptionLabel->setText(description);
}

///
/// \brief AboutDataWidget::setLinkUrl
/// \param url
///
void AboutDataWidget::setLinkUrl(const QUrl &url)
{
    _linkUrl = url;
    updateLinkButton();
}

///
/// \brief AboutDataWidget::setLinkIcon
/// \param icon
///
void AboutDataWidget::setLinkIcon(const QIcon &icon)
{
    _linkButton->setIcon(icon);
}

///
/// \brief AboutDataWidget::setLinkToolTip
/// \param toolTip
///
void AboutDataWidget::setLinkToolTip(const QString &toolTip)
{
    _linkButton->setToolTip(toolTip);
}

///
/// \brief AboutDataWidget::title
/// \return
///
QString AboutDataWidget::title() const
{
    return _titleLabel->text();
}

///
/// \brief AboutDataWidget::version
/// \return
///
QString AboutDataWidget::version() const
{
    QString versionText = _versionLabel->text();
    if (versionText.startsWith('(') && versionText.endsWith(')')) {
        return versionText.mid(1, versionText.length() - 2);
    }
    return versionText;
}

///
/// \brief AboutDataWidget::description
/// \return
///
QString AboutDataWidget::description() const
{
    return _descriptionLabel->text();
}

///
/// \brief AboutDataWidget::linkUrl
/// \return
///
QUrl AboutDataWidget::linkUrl() const
{
    return _linkUrl;
}

///
/// \brief AboutDataWidget::linkToolTip
/// \return
///
QString AboutDataWidget::linkToolTip() const
{
    return _linkButton->toolTip();
}

///
/// \brief AboutDataWidget::on_linkClicked
///
void AboutDataWidget::on_linkClicked()
{
    if (_linkUrl.isValid()) {
        QDesktopServices::openUrl(_linkUrl);
    }
}

///
/// \brief AboutDataWidget::updateLinkButton
///
void AboutDataWidget::updateLinkButton()
{
    const bool shouldBeVisible = _linkUrl.isValid() && !_linkUrl.isEmpty();
    _linkButton->setVisible(shouldBeVisible);
    _linkButton->setToolTip(_linkUrl.toString());
}
