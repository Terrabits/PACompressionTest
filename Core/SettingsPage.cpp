#include "SettingsPage.h"
#include "ui_SettingsPage.h"


// Project
#include "Settings.h"

// RsaToolbox
#include <General.h>
using namespace RsaToolbox;


SettingsPage::SettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage),
    _vna(NULL),
    _keys(NULL)
{
    ui->setupUi(this);

    // Frequency Inputs
    ui->startFrequency->setParameterName("Start frequency");
    connect(ui->startFrequency, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->startFrequency, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->stopFrequency->setParameterName("Stop frequency");
    connect(ui->stopFrequency, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->stopFrequency, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->frequencyPoints->setParameterName("Frequency points");
    connect(ui->frequencyPoints, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->frequencyPoints, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->ifBw->setParameterName("IF BW");
    connect(ui->ifBw, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->ifBw, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    // Power Inputs
    ui->startPower->setParameterName("Start power");
    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->startPower, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->stopPower->setParameterName("Stop power");
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->stopPower, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->powerPoints->setParameterName("Power points");
    connect(ui->powerPoints, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->powerPoints, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->compression->setParameterName("Compression level");
    connect(ui->compression, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->compression, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    connect(ui->startPower, SIGNAL(powerChanged(double)),
            this, SLOT(updatePowerStepSize()));
    connect(ui->stopPower, SIGNAL(powerChanged(double)),
            this, SLOT(updatePowerStepSize()));
    connect(ui->powerPoints, SIGNAL(pointsChanged(double)),
            this, SLOT(updatePowerStepSize()));

    // Miscellaneous
    ui->outputPort->setParameterName("Output port");
    connect(ui->outputPort, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->outputPort, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    ui->inputPort->setParameterName("Input port");
    connect(ui->inputPort, SIGNAL(outOfRange(QString)),
            ui->error, SLOT(showMessage(QString)));
    connect(ui->inputPort, SIGNAL(outOfRange(QString)),
            this, SIGNAL(inputError()));

    // Buttons
    connect(ui->exportButton, SIGNAL(clicked()),
            this, SIGNAL(exportClicked()));
    connect(ui->miniGuiButton, SIGNAL(clicked()),
            this, SIGNAL(miniGuiClicked()));
    connect(ui->closeButton, SIGNAL(clicked()),
            this, SIGNAL(closeClicked()));
    connect(ui->measureButton, SIGNAL(clicked()),
            this, SIGNAL(measureClicked()));
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::setVna(Vna *vna) {
    _vna = vna;

    const double minimumFrequency_Hz = _vna->properties().minimumFrequency_Hz();
    const double maximumFrequency_Hz = _vna->properties().maximumFrequency_Hz();

    const double minimumPower_dBm = _vna->properties().minimumPower_dBm();
    const double maximumPower_dBm = _vna->properties().maximumPower_dBm();

    const uint maximumPoints = _vna->properties().maximumPoints();

    const uint ports = _vna->testPorts();

    // Frequency limits
    ui->startFrequency->setMinimum(minimumFrequency_Hz);
    ui->startFrequency->setMaximum(maximumFrequency_Hz);
    ui->stopFrequency->setMinimum(minimumFrequency_Hz);
    ui->stopFrequency->setMaximum(maximumFrequency_Hz);
    ui->frequencyPoints->setMinimum(2);
    ui->frequencyPoints->setMaximum(maximumPoints);
    ui->ifBw->setAcceptedValues(_vna->properties().ifBandwidthValues_Hz());

    // Power limits
    ui->startPower->setMinimum(minimumPower_dBm);
    ui->startPower->setMaximum(maximumPower_dBm);
    ui->stopPower->setMinimum(minimumPower_dBm);
    ui->stopPower->setMaximum(maximumPower_dBm);
    ui->powerPoints->setMinimum(2);
    ui->powerPoints->setMaximum(maximumPoints);
    ui->compression->setMinimum(0.01);

    // Miscellaneous
    ui->channel->clear();
    ui->channel->addItems(toStringList(_vna->channels()));
    ui->outputPort->setMinimum(1);
    ui->outputPort->setMaximum(ports);
    ui->inputPort->setMinimum(1);
    ui->inputPort->setMaximum(ports);
}

void SettingsPage::setKeys(Keys *keys) {
    _keys = keys;
}
void SettingsPage::loadKeys() {
    bool _bool;
    quint32 _quint32;
    double _double;
    QString _string;

    // Frequency
    if (_keys->exists(START_FREQUENCY_KEY)) {
        _keys->get(START_FREQUENCY_KEY, _double);
        ui->startFrequency->setFrequency(_double);
    }
    if (_keys->exists(STOP_FREQUENCY_KEY)) {
        _keys->get(STOP_FREQUENCY_KEY, _double);
        ui->stopFrequency->setFrequency(_double);
    }
    if (_keys->exists(FREQUENCY_POINTS_KEY)) {
        _keys->get(FREQUENCY_POINTS_KEY, _quint32);
        ui->frequencyPoints->setPoints(_quint32);
    }
    if (_keys->exists(IF_BW_KEY)) {
        _keys->get(IF_BW_KEY, _double);
        ui->ifBw->setFrequency(_double);
    }

    // Power
    if (_keys->exists(START_POWER_KEY)) {
        _keys->get(START_POWER_KEY, _double);
        ui->startPower->setPower(_double);
    }
    if (_keys->exists(STOP_POWER_KEY)) {
        _keys->get(STOP_POWER_KEY, _double);
        ui->stopPower->setPower(_double);
    }
    if (_keys->exists(POWER_POINTS_KEY)) {
        _keys->get(POWER_POINTS_KEY, _quint32);
        ui->powerPoints->setPoints(_quint32);
    }
    if (_keys->exists(COMPRESSION_LEVEL_KEY)) {
        _keys->get(COMPRESSION_LEVEL_KEY, _double);
        ui->compression->setValue(_double);
    }
    if (_keys->exists(IS_GAIN_EXPANSION_KEY)) {
        _keys->get(IS_GAIN_EXPANSION_KEY, _bool);
        ui->includeGainExpansion->setChecked(_bool);
    }
    if (_keys->exists(IS_STOP_AT_COMPRESSION_KEY)) {
        _keys->get(IS_STOP_AT_COMPRESSION_KEY, _bool);
        ui->stopAtCompression->setChecked(_bool);
    }
    if (_keys->exists(POST_CONDITION_KEY)) {
        _keys->get(POST_CONDITION_KEY, _string);
        ui->postCondition->setCurrentText(_string);
    }

    // Miscellaneous
    if (_keys->exists(CHANNEL_KEY)) {
        _keys->get(CHANNEL_KEY, _string);
        if (ui->channel->findText(_string) != -1)
            ui->channel->setCurrentText(_string);
    }
    if (_keys->exists(OUTPUT_PORT_KEY)) {
        _keys->get(OUTPUT_PORT_KEY, _quint32);
        ui->outputPort->setPoints(_quint32);
    }
    if (_keys->exists(INPUT_PORT_KEY)) {
        _keys->get(INPUT_PORT_KEY, _quint32);
        ui->inputPort->setPoints(_quint32);
    }
    if (_keys->exists(SWEEP_TYPE_KEY)) {
        _keys->get(SWEEP_TYPE_KEY, _string);
        if (ui->sweepType->findText(_string) != -1)
            ui->sweepType->setCurrentText(_string);
    }
}
void SettingsPage::saveKeys() const {
    // Assumes valid input

    // Frequency
    _keys->set(START_FREQUENCY_KEY, ui->startFrequency->frequency_Hz());
    _keys->set(STOP_FREQUENCY_KEY, ui->stopFrequency->frequency_Hz());
    _keys->set(FREQUENCY_POINTS_KEY, quint32(ui->frequencyPoints->points()));
    _keys->set(IF_BW_KEY, ui->ifBw->frequency_Hz());

    // Power
    _keys->set(START_POWER_KEY, ui->startPower->power_dBm());
    _keys->set(STOP_POWER_KEY, ui->stopPower->power_dBm());
    _keys->set(POWER_POINTS_KEY, quint32(ui->powerPoints->points()));
    _keys->set(COMPRESSION_LEVEL_KEY, ui->compression->value_dB());
    _keys->set(IS_GAIN_EXPANSION_KEY, ui->includeGainExpansion->isChecked());
    _keys->set(IS_STOP_AT_COMPRESSION_KEY, ui->stopAtCompression->isChecked());
    _keys->set(POST_CONDITION_KEY, ui->postCondition->currentText());

    // Miscellaneous
    _keys->set(CHANNEL_KEY, ui->channel->currentText());
    _keys->set(OUTPUT_PORT_KEY, quint32(ui->outputPort->points()));
    _keys->set(INPUT_PORT_KEY, quint32(ui->inputPort->points()));
    _keys->set(SWEEP_TYPE_KEY, ui->sweepType->currentText());
}

bool SettingsPage::hasAcceptableInput() {
    // Frequency
    if (!ui->startFrequency->hasAcceptableInput()) {
        ui->error->showMessage("*Enter start frequency");
        ui->startFrequency->selectAll();
        ui->startFrequency->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->stopFrequency->hasAcceptableInput()) {
        ui->error->showMessage("*Enter stop frequency");
        ui->stopFrequency->selectAll();
        ui->stopFrequency->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->frequencyPoints->hasAcceptableInput()) {
        ui->error->showMessage("*Enter frequency points");
        ui->frequencyPoints->selectAll();
        ui->frequencyPoints->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->ifBw->hasAcceptableInput()) {
        ui->error->showMessage("*Enter IF BW");
        ui->ifBw->selectAll();
        ui->ifBw->setFocus();
        emit inputError();
        return false;
    }

    // Power
    if (!ui->startPower->hasAcceptableInput()) {
        ui->error->showMessage("*Enter start power");
        ui->startPower->selectAll();
        ui->startPower->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->stopPower->hasAcceptableInput()) {
        ui->error->showMessage("*Enter stop power");
        ui->stopPower->selectAll();
        ui->stopPower->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->powerPoints->hasAcceptableInput()) {
        ui->error->showMessage("*Enter power points");
        ui->powerPoints->selectAll();
        ui->powerPoints->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->compression->hasAcceptableInput()) {
        ui->error->showMessage("*Enter compression level");
        ui->compression->selectAll();
        ui->compression->setFocus();
        emit inputError();
        return false;
    }

    // Miscellaneous
    if (!ui->outputPort->hasAcceptableInput()) {
        ui->error->showMessage("*Enter output port");
        ui->outputPort->selectAll();
        ui->outputPort->setFocus();
        emit inputError();
        return false;
    }
    if (!ui->inputPort->hasAcceptableInput()) {
        ui->error->showMessage("*Enter input port");
        ui->inputPort->selectAll();
        ui->inputPort->setFocus();
        emit inputError();
        return false;
    }

    // isValid
    QString message;
    if (!settings().isValid(*_vna, message)) {
        ui->error->showMessage(message);
        emit inputError();
        return false;
    }

    return true;
}
MeasurementSettings SettingsPage::settings() const {
    MeasurementSettings _settings;

    // Frequency
    _settings.setStartFrequency(ui->startFrequency->frequency_Hz());
    _settings.setStopFrequency(ui->stopFrequency->frequency_Hz());
    _settings.setFrequencyPoints(ui->frequencyPoints->points());
    _settings.setIfBw(ui->ifBw->frequency_Hz());

    // Power
    _settings.setStartPower(ui->startPower->power_dBm());
    _settings.setStopPower(ui->stopPower->power_dBm());
    _settings.setStopAtCompression(ui->stopAtCompression->isChecked());
    _settings.setPowerPoints(ui->powerPoints->points());
    _settings.setCompressionLevel(ui->compression->value_dB());
    _settings.setGainExpansion(ui->includeGainExpansion->isChecked());

    // Miscellaneous
    _settings.setChannel(ui->channel->currentText().toUInt());
    _settings.setOutputPort(ui->outputPort->points());
    _settings.setInputPort(ui->inputPort->points());
    _settings.setRfOffPostCondition(ui->postCondition->currentText().compare("RF Off", Qt::CaseInsensitive) == 0);
    if (ui->sweepType->currentText() == "Frequency")
        _settings.setSweepType(MeasurementSettings::SweepType::Frequency);
    else
        _settings.setSweepType(MeasurementSettings::SweepType::Power);

    return _settings;
}

ErrorLabel *SettingsPage::errorLabel() {
    return ui->error;
}

void SettingsPage::enableExport() {
    ui->exportButton->setEnabled(true);
}
void SettingsPage::disableExport() {
    ui->exportButton->setDisabled(true);
}

void SettingsPage::updatePowerStepSize() {
    bool isValidInput = true;
    isValidInput = isValidInput && ui->startPower->hasAcceptableInput();
    isValidInput = isValidInput && ui->stopPower->hasAcceptableInput();
    isValidInput = isValidInput && ui->powerPoints->hasAcceptableInput();
    if (!isValidInput) {
        ui->powerSpacing->clear();
        return;
    }

    const double start = ui->startPower->power_dBm();
    const double stop = ui->stopPower->power_dBm();
    if (start >= stop) {
        ui->powerSpacing->clear();
        return;
    }

    const uint points = ui->powerPoints->points();
    const double spacing = (stop - start) / (points - 1);
    ui->powerSpacing->setText(formatDouble(spacing, 3) + " dBm");
}
