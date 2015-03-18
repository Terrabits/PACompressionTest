#include "TestPlan.h"


// RsaToolbox
using namespace RsaToolbox;

// Qt
#include <QDebug>


TestPlan::TestPlan(QObject *parent) : QObject(parent)
{
    _vna = NULL;
    initialize();
}
TestPlan::TestPlan(const TestPlan &other) {
    setEqual(other);
}

TestPlan::~TestPlan()
{

}

bool TestPlan::isVna() const {
    if (_vna == NULL)
        return false;
    return _vna->isConnected();
}
Vna *TestPlan::vna() const {
    return _vna;
}
void TestPlan::setVna(Vna *vna) {
    if (vna != _vna) {
        _vna = vna;
        emit vnaChanged(_vna);
    }
}

bool TestPlan::isValid() {
//    if (!isVna()) {
//        emit error("No VNA conection.");
//        return false;
//    }

    if (!isFrequencyValid(_startFrequency_Hz))
        return false;
    if (!isFrequencyValid(_stopFrequency_Hz))
        return false;
    if (!(_startFrequency_Hz < _stopFrequency_Hz)) {
        emit error("*Start frequency must be less than stop frequency.");
        return false;
    }
    if (!isPointsValid(_frequencyPoints))
        return false;

    if (!isPowerValid(_startPower_dBm))
        return false;
    if (!isPowerValid(_absoluteStopPower_dBm))
        return false;
    if (!(_startPower_dBm < _absoluteStopPower_dBm)) {
        emit error("*Start power must be less than absolute stop power.");
        return false;
    }
    if (!isStepSizeValid(_powerStepSize_dBm))
        return false;

    if (!isCompressionValueValid(_compression_dB))
        return false;

    return true;
}

double TestPlan::startFrequency_Hz() const {
    return _startFrequency_Hz;
}
bool TestPlan::setStartFrequency(double frequency, SiPrefix prefix) {
    qDebug() << "Start frequency: " << frequency << toString(prefix, Units::Hertz);
    double f = frequency * toDouble(prefix);
    if (!isFrequencyValid(f))
        return false;
    if (f == _startFrequency_Hz)
        return true;

    _startFrequency_Hz = f;
    emit startFrequencyChanged(_startFrequency_Hz);
    return true;
}

double TestPlan::stopFrequency_Hz() const {
    return _stopFrequency_Hz;
}
bool TestPlan::setStopFrequency(double frequency, SiPrefix prefix) {
    qDebug() << "Stop frequency: " << frequency << toString(prefix, Units::Hertz);
    double f = frequency * toDouble(prefix);
    if (!isFrequencyValid(f))
        return false;
    if (f == _stopFrequency_Hz)
        return true;

    _stopFrequency_Hz = f;
    emit stopFrequencyChanged(_stopFrequency_Hz);
    return true;
}

uint TestPlan::frequencyPoints() const {
    return _frequencyPoints;
}
bool TestPlan::setFrequencyPoints(uint points) {
    qDebug() << "Points: " << points;
    if (!isPointsValid(points))
        return false;
    if (points == _frequencyPoints)
        return true;

    _frequencyPoints = points;
    emit frequencyPointsChanged(_frequencyPoints);
    return true;
}

double TestPlan::startPower_dBm() const {
    return _startPower_dBm;
}
bool TestPlan::setStartPower(double power_dBm) {
    qDebug() << "Start Power: " << power_dBm << "dBm";
    if (!isPowerValid(power_dBm))
        return false;
    if (power_dBm == _startPower_dBm)
        return true;


    _startPower_dBm = power_dBm;
    emit startPowerChanged(_startPower_dBm);
    return true;
}

double TestPlan::powerStepSize_dBm() const {
    return _powerStepSize_dBm;
}
bool TestPlan::setPowerStepSize(double stepSize_dBm) {
    qDebug() << "Step size: " << stepSize_dBm << "dBm";
    if (!isStepSizeValid(stepSize_dBm))
        return false;
    if (stepSize_dBm == _powerStepSize_dBm)
        return true;


    _powerStepSize_dBm = stepSize_dBm;
    emit powerStepSizeChanged(_powerStepSize_dBm);
    return true;
}

double TestPlan::absoluteStopPower_dBm() const {
    return _absoluteStopPower_dBm;
}
bool TestPlan::setAbsoluteStopPower(double power_dBm) {
    qDebug() << "Stop Power: " << power_dBm << "dBm";
    if (!isPowerValid(power_dBm))
        return false;
    if (power_dBm == _absoluteStopPower_dBm)
        return true;



    _absoluteStopPower_dBm = power_dBm;
    emit absoluteStopPowerChanged(_absoluteStopPower_dBm);
    return true;
}

double TestPlan::compressionValue_dB() const {
    return _compression_dB;
}
bool TestPlan::setCompressionValue(double value_dB) {
    qDebug() << "Compression: " << value_dB << "dB";
    if (!isCompressionValueValid(value_dB))
        return false;
    if (value_dB == _compression_dB)
        return true;


    _compression_dB = value_dB;
    emit compressionValueChanged(_compression_dB);
    return true;
}

void TestPlan::operator=(const TestPlan &other) {
    setEqual(other);
}
void TestPlan::setEqual(const TestPlan &other) {
    _startFrequency_Hz = other._startFrequency_Hz;
    _stopFrequency_Hz = other._stopFrequency_Hz;
    _frequencyPoints = other._frequencyPoints;

    _startPower_dBm = other._startPower_dBm;
    _powerStepSize_dBm = other._powerStepSize_dBm;
    _absoluteStopPower_dBm = other._absoluteStopPower_dBm;

    _compression_dB = other._compression_dB;
}

void TestPlan::initialize() {
    _startFrequency_Hz = 0;
    _stopFrequency_Hz = 0;
    _frequencyPoints = 0;

    _startPower_dBm = 0;
    _powerStepSize_dBm = 0;
    _absoluteStopPower_dBm = 0;

    _compression_dB = 0;

    emit reset();
}

bool TestPlan::isFrequencyValid(double hz) {
    if (hz < 0) {
        emit error("*Frequency must be greater than zero");
        return false;
    }
    else if (isVna() && hz < _vna->properties().minimumFrequency_Hz()) {
        QString msg = "*Frequency must be greater than or equal to %1";
        msg = msg.arg(formatValue(_vna->properties().minimumFrequency_Hz(), 1, Units::Hertz));
        emit error(msg);
        return false;
    }
    else if (isVna() && hz > _vna->properties().maximumFrequency_Hz()) {
        QString msg = "*Frequency must be less than or equal to %1";
        msg = msg.arg(formatValue(_vna->properties().maximumFrequency_Hz(), 1, Units::Hertz));
        emit error(msg);
        return false;
    }

    return true;
}
bool TestPlan::isPointsValid(uint points) {
    if (points == 0) {
        emit error("*Points must be greater than zero.");
        return false;
    }
    else if (isVna() && points > _vna->properties().maximumPoints()) {
        QString msg = "*Points must be less than or equal to %1";
        msg = msg.arg(_vna->properties().maximumPoints());
        emit error(msg);
        return false;
    }

    return true;
}

bool TestPlan::isPowerValid(double dBm) {
    if (isVna() && dBm < _vna->properties().minimumPower_dBm()) {
        QString msg = "*Power must be greater than or equal to %1";
        msg = msg.arg(formatValue(_vna->properties().minimumPower_dBm(), 1, Units::dBm));
        emit error(msg);
        return false;
    }
    if (isVna() && dBm > _vna->properties().maximumPower_dBm()) {
        QString msg = "*Power must be less than or equal to %1";
        msg = msg.arg(formatValue(_vna->properties().maximumPower_dBm(), 1, Units::dBm));
        emit error(msg);
        return false;
    }

    return true;
}
bool TestPlan::isStepSizeValid(double dBm) {
    if (dBm <= 0) {
        emit error("*Power step size must be greater than zero.");
        return false;
    }

    return true;
}

bool TestPlan::isCompressionValueValid(double dB) {
    if (dB <= 0) {
        emit error("*Compression value must be greater than zero.");
        return false;
    }

    return true;
}

