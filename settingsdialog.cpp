#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QSettings settings("Devesh Rai", "Easy Picker");
    ui->xCameraOffsetLE->setText(QString::number(settings.value("settings/xCameraOffset").toDouble()));
    ui->yCameraOffsetLE->setText(QString::number(settings.value("settings/yCameraOffset").toDouble()));


}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(refreshPressed)
    {
        this->machineSerial=arg1;
        QSettings settings("Devesh Rai", "Easy Picker");
        settings.setValue("settings/machineSerialName",arg1);
    }
}

void SettingsDialog::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if(refreshPressed)
    {
        this->vacuumSerial=arg1;
        QSettings settings("Devesh Rai", "Easy Picker");
        settings.setValue("settings/vacuumSerialName",arg1);
    }
}

void SettingsDialog::on_pushButton_clicked()
{
    ui->comboBox->clear();
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QSerialPortInfo port;

    foreach(port, com_ports)
    {
        ui->comboBox->addItem(port.portName());
    }
    this->refreshPressed=true;

}

void SettingsDialog::on_pushButton_2_clicked()
{
    ui->comboBox_2->clear();
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QSerialPortInfo port;

    foreach(port, com_ports)
    {

        ui->comboBox_2->addItem(port.portName());
    }
    this->refreshPressed=true;

}

void SettingsDialog::on_pushButton_3_clicked()
{
    QSettings settings("Devesh Rai", "Easy Picker");
    settings.setValue("settings/xCameraOffset",ui->xCameraOffsetLE->text().toDouble());
    settings.setValue("settings/yCameraOffset",ui->yCameraOffsetLE->text().toDouble());

}
