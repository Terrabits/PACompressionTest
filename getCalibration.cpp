#include "getCalibration.h"
#include "ui_getCalibration.h"

getCalibration::getCalibration(RsaToolbox::Vna *vna, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getCalibration),
    _vna(vna),
    _selectedCal("None")
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()),
            this, SLOT(processCal()));
}

getCalibration::~getCalibration()
{
    delete ui;
}

bool getCalibration::isCalSelected() const {
    return !_selectedCal.isEmpty() && _selectedCal != "None";
}
QString getCalibration::selectedCal() const {
    if (!isCalSelected())
        return QString();

    return _selectedCal;
}
int getCalibration::exec() {
    _calibrations.clear();
    _calibrations << "None";
    _calibrations << _vna->calGroups();
    ui->calibrations->clear();
    ui->calibrations->addItems(_calibrations);
    if (isCalSelected()) {
        int row = _calibrations.indexOf(_selectedCal);
        if (row != -1)
            ui->calibrations->setCurrentRow(row);
    }
    else {
        ui->calibrations->setCurrentRow(0);
    }
    return QDialog::exec();
}

void getCalibration::processCal() {
    int row = ui->calibrations->currentRow();
    if (row != 0)
        _selectedCal = _calibrations[row];
}
