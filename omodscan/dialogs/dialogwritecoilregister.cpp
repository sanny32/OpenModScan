#include "modbuslimits.h"
#include "dialogcoilsimulation.h"
#include "dialogwritecoilregister.h"
#include "ui_dialogwritecoilregister.h"

///
/// \brief DialogWriteCoilRegister::DialogWriteCoilRegister
/// \param params
/// \param parent
///
DialogWriteCoilRegister::DialogWriteCoilRegister(ModbusWriteParams& params, ModbusSimulationParams& simParams, bool hexAddress, QWidget *parent) :
    QFixedSizeDialog(parent),
    ui(new Ui::DialogWriteCoilRegister)
    ,_writeParams(params)
    ,_simParams(simParams)
{
    ui->setupUi(this);
    ui->lineEditNode->setInputRange(ModbusLimits::slaveRange());
    ui->lineEditAddress->setInputMode(hexAddress ? NumericLineEdit::HexMode : NumericLineEdit::Int32Mode);
    ui->lineEditAddress->setInputRange(ModbusLimits::addressRange(params.ZeroBasedAddress));
    ui->lineEditNode->setValue(params.Node);
    ui->lineEditAddress->setValue(params.Address);
    ui->radioButtonOn->setChecked(params.Value.toBool());
    ui->radioButtonOff->setChecked(!params.Value.toBool());
    ui->pushButtonSimulation->setVisible(simParams.Mode != SimulationMode::No);

    if(simParams.Mode != SimulationMode::No)
    {
        QLabel* iconLabel = new QLabel(ui->pushButtonSimulation);
        iconLabel->setPixmap(QIcon(":/res/pointGreen.png").pixmap(4, 4));
        iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        QLabel* textLabel = new QLabel(ui->pushButtonSimulation->text(), ui->pushButtonSimulation);
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        auto layout = new QHBoxLayout(ui->pushButtonSimulation);
        layout->setContentsMargins(4,0,4,0);
        layout->addWidget(iconLabel);
        layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
        layout->addWidget(textLabel);
        layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

        ui->pushButtonSimulation->setText(QString());
        ui->pushButtonSimulation->setLayout(layout);
    }

    if(ui->radioButtonOff->isChecked())
        ui->radioButtonOn->setFocus();
    else
        ui->radioButtonOff->setFocus();
}

///
/// \brief DialogWriteCoilRegister::~DialogWriteCoilRegister
///
DialogWriteCoilRegister::~DialogWriteCoilRegister()
{
    delete ui;
}

///
/// \brief DialogWriteCoilRegister::accept
///
void DialogWriteCoilRegister::accept()
{
    _writeParams.Address = ui->lineEditAddress->value<int>();
    _writeParams.Value = ui->radioButtonOn->isChecked();
    _writeParams.Node = ui->lineEditNode->value<int>();

    QFixedSizeDialog::accept();
}

///
/// \brief DialogWriteCoilRegister::on_pushButtonSimulation_clicked
///
void DialogWriteCoilRegister::on_pushButtonSimulation_clicked()
{
    DialogCoilSimulation dlg(_simParams, this);
    if(dlg.exec() == QDialog::Accepted) done(2);
}
