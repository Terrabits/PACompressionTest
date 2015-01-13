#include "getcalibration.h"
#include "ui_getcalibration.h"

GetCalibration::GetCalibration(QStringList calibrations,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetCalibration)
{
    ui->setupUi(this);
    ui->listWidget->clear();
    ui->listWidget->addItems(calibrations);
    ui->listWidget->setCurrentRow(0);
    _calibration = "";
}

GetCalibration::~GetCalibration() {
    delete ui;
}

QString GetCalibration::calibration() {
    return(_calibration);
}

void GetCalibration::on_pushButton_clicked() {
    _calibration = ui->listWidget->currentItem()->text();
    this->close();
}
