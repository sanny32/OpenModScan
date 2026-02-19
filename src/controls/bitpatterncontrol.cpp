#include "bitpatterncontrol.h"
#include "ui_bitpatterncontrol.h"

///
/// \brief BitPatternControl::BitPatternControl
/// \param parent
///
BitPatternControl::BitPatternControl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BitPatternControl)
{
    ui->setupUi(this);
    for (int i = 0; i < 16; i++)
    {
        auto chb = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(chb) connect(chb, &QCheckBox::stateChanged, this, &BitPatternControl::on_checkStateChanged);

        auto lbl = findChild<ClickableLabel*>(QString("label%1").arg(i));
        if(chb && lbl) connect(lbl, &ClickableLabel::clicked, chb, &QCheckBox::toggle);
    }
}

///
/// \brief BitPatternControl::~BitPatternControl
///
BitPatternControl::~BitPatternControl()
{
    delete ui;
}

///
/// \brief BitPatternControl::value
/// \return
///
quint16 BitPatternControl::value() const
{
    quint16 value = 0;
    for (int i = 0; i < 16; i++)
    {
        auto chb = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(chb) value |= chb->isChecked() << i;
    }

    return value;
}

///
/// \brief BitPatternControl::setValue
/// \param value
///
void BitPatternControl::setValue(quint16 value)
{
    for (int i = 0; i < 16; i++)
    {
        auto chb = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if(chb) chb->setChecked(value >> i & 1);
    }
}

///
/// \brief BitPatternControl::on_checkStateChanged
///
void BitPatternControl::on_checkStateChanged(int)
{
    emit valueChanged(value());
}
