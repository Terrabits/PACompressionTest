#include "TestPlan.h"


// RsaToolbox
using namespace RsaToolbox;

TestPlan::TestPlan(QObject *parent) : QObject(parent)
{
    _vna = NULL;
    initialize();
}

TestPlan::~TestPlan()
{

}

bool TestPlan::isVna() const {
    return _vna != NULL;
}
Vna TestPlan::vna() const {
    return _vna;
}
void TestPlan::setVna(Vna *vna) {
    if (vna != _vna) {
        _vna = vna;
        emit vnaChanged();
    }
}

double TestPlan::startFrequency_Hz() const {
    return _startFrequency_Hz;
}
void TestPlan::setStartFrequency(double frequency, SiPrefix prefix) {
    double f = frequency * toDouble(prefix);
    if (f == _startFrequency_Hz)
        return;

        if (f < 0) {
            emit error("Frequency must be greater than zero");
        }
        else if (isVna() && f < _vna->properties().minimumFrequency_Hz()) {
            QString msg = "Frequency must be greater than %1";
            msg = msg.arg(formatValue(_vna->properties().minimumFrequency_Hz(), 0, Units::Hertz));
            emit error(msg);
        }
        else {
            _startFrequency_Hz = f;
            emit startFrequencyChanged(_startFrequency_Hz);
        }
}

double TestPlan::stopFrequency_Hz() const {
    return _stopFrequency_Hz;
}
void TestPlan::setStopFrequency(double frequency, SiPrefix prefix) {
    double f = frequency * toDouble(prefix);
    if (f == _stopFrequency_Hz)
        return;

    if (f < 0) {
        emit error("Frequency must be greater than zero");
    }
    else if(isVna() && f > _vna->properties().maximumFrequency_Hz()) {

    }
    else {
        _stopFrequency_Hz = f;
        emit stopFrequencyChanged(_stopFrequency_Hz);
    }
}

uint TestPlan::frequencyPoints() const {
    return _frequencyPoints;
}
void TestPlan::setFrequencyPoints(uint points) {
    if (points != _frequencyPoints) {
        _frequencyPoints = points;
        emit frequencyPointsChanged(_frequencyPoints);
    }
}

double TestPlan::startPower_dBm() const {
    return _startPower_dBm;
}
void TestPlan::setStartPower(double power_dBm) {
    if (_startPower_dBm != power_dBm) {
        _startPower_dBm = power_dBm;
        emit startPowerChanged(_startPower_dBm);
    }
}

double TestPlan::powerStepSize_dBm() const {
    return _powerStepSize_dBm;
}
void TestPlan::setPowerStepSize(double stepSize_dBm) {
    if (stepSize_dBm != _powerStepSize_dBm) {
        _powerStepSize_dBm = stepSize_dBm;
        emit powerStepSizeChanged(_powerStepSize_dBm);
    }
}

double TestPlan::absoluteStopPower_dBm() const {
    return _absoluteStopPower_dBm;
}
void TestPlan::setAbsoluteStopPower(double power_dBm) {
    if (power_dBm != _absoluteStopPower_dBm) {
        _absoluteStopPower_dBm = power_dBm;
        emit absoluteStopPowerChanged(_absoluteStopPower_dBm);
    }
}

double TestPlan::compressionPoint_dB() const {
    return _compression_dB;
}
void TestPlan::setCompressionPoint(double value_dB) {
    if (value_dB != _compression_dB) {
        _compression_dB = value_dB;
        emit compressionPointChanged(_compression_dB);
    }
}

void TestPlan::initialize() {
    _startFrequency_Hz = 0;
    _stopFrequency_Hz = 0;
    _frequencyPoints = 0;

    _startPower_dBm = 0;
    _powerStepSize_dBm = 0;
    _absoluteStopPower_dBm = 0;

    _compression_dB = 0;
}

