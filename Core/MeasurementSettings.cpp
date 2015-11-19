#include "MeasurementSettings.h"


// RsaToolbox
#include <General.h>
using namespace RsaToolbox;

// Qt
#include <QFile>


MeasurementSettings::MeasurementSettings() :
    _startFreq_Hz(0),
    _stopFreq_Hz(1.0E12),
    _frequencyPoints(101),
    _ifBw_Hz(1.0E3),
    _startPower_dBm(-20.0),
    _stopPower_dBm(0),
    _powerPoints(81),
    _compressionLevel_dB(1.0),
    _isGainExpansion(true),
    _isStopAtCompression(true),
    _isRfOffPostCondition(false),
    _channel(1),
    _outputPort(2),
    _inputPort(1)
{

}

MeasurementSettings::~MeasurementSettings()
{

}

double MeasurementSettings::startFrequency_Hz() const {
    return _startFreq_Hz;
}
void MeasurementSettings::setStartFrequency(double frequency_Hz) {
    _startFreq_Hz = frequency_Hz;
}
double MeasurementSettings::stopFrequency_Hz() const {
    return _stopFreq_Hz;
}
void MeasurementSettings::setStopFrequency(double frequency_Hz) {
    _stopFreq_Hz = frequency_Hz;
}
uint MeasurementSettings::frequencyPoints() const {
    return _frequencyPoints;
}
void MeasurementSettings::setFrequencyPoints(uint points) {
    _frequencyPoints = points;
}
double MeasurementSettings::ifBw_Hz() const {
    return _ifBw_Hz;
}
void MeasurementSettings::setIfBw(double bandwidth_Hz) {
    _ifBw_Hz = bandwidth_Hz;
}

double MeasurementSettings::startPower_dBm() const {
    return _startPower_dBm;
}
void MeasurementSettings::setStartPower(double power_dBm) {
    _startPower_dBm = power_dBm;
}
double MeasurementSettings::stopPower_dBm() const {
    return _stopPower_dBm;
}
void MeasurementSettings::setStopPower(double power_dBm) {
    _stopPower_dBm = power_dBm;
}
uint MeasurementSettings::powerPoints() const {
    return _powerPoints;
}
void MeasurementSettings::setPowerPoints(uint points) {
    _powerPoints = points;
}
double MeasurementSettings::compressionLevel_dB() const {
    return _compressionLevel_dB;
}
void MeasurementSettings::setCompressionLevel(double level_dB) {
    _compressionLevel_dB = level_dB;
}
bool MeasurementSettings::isGainExpansion() const {
    return _isGainExpansion;
}
void MeasurementSettings::setGainExpansion(bool includeGainExpansion) {
    _isGainExpansion = includeGainExpansion;
}
bool MeasurementSettings::isStopAtCompression() const {
    return _isStopAtCompression;
}
void MeasurementSettings::setStopAtCompression(bool stopAtCompression) {
    _isStopAtCompression = stopAtCompression;
}
bool MeasurementSettings::isRfOffPostCondition() const {
    return _isRfOffPostCondition;
}
void MeasurementSettings::setRfOffPostCondition(bool rfOff) {
    _isRfOffPostCondition = rfOff;
}

uint MeasurementSettings::channel() const {
    return _channel;
}
void MeasurementSettings::setChannel(uint index) {
    _channel = index;
}
uint MeasurementSettings::outputPort() const {
    return _outputPort;
}
void MeasurementSettings::setOutputPort(uint port) {
    _outputPort = port;
}
uint MeasurementSettings::inputPort() const {
    return _inputPort;
}
void MeasurementSettings::setInputPort(uint port) {
    _inputPort = port;
}

bool MeasurementSettings::isValid(Vna &vna) const {
    QString msg;
    return isValid(vna, msg);
}
bool MeasurementSettings::isValid(Vna &vna, QString &errorMessage) const {
    // Vna properties
    const double minFreq_Hz = vna.properties().minimumFrequency_Hz();
    const double maxFreq_Hz = vna.properties().maximumFrequency_Hz();
    const QRowVector ifBws = vna.properties().ifBandwidthValues_Hz();
    const double minPower_dBm = vna.properties().minimumPower_dBm();
    const double maxPower_dBm = vna.properties().maximumPower_dBm();
    const uint maxPoints = vna.properties().maximumPoints();
    const uint testPorts = vna.testPorts();

    // Frequency
    if (_startFreq_Hz < minFreq_Hz) {
        errorMessage = "*Start frequency must be at least %1";
        errorMessage = errorMessage.arg(formatValue(minFreq_Hz, 1, Units::Hertz));
        return false;
    }
    if (_stopFreq_Hz > maxFreq_Hz) {
        errorMessage = "*Stop frequency must be at most %1";
        errorMessage = errorMessage.arg(formatValue(maxFreq_Hz, 1, Units::Hertz));
        return false;
    }
    if (_startFreq_Hz >= _stopFreq_Hz) {
        errorMessage = "*Start frequency must be less than stop frequency.";
        return false;
    }
    if (!_frequencyPoints) {
        errorMessage = "*Frequency points must be at least 1";
        return false;
    }
    if (_frequencyPoints > vna.properties().maximumPoints()) {
        errorMessage = "*Frequency points must be at most %1";
        errorMessage = errorMessage.arg(maxPoints);
        return false;
    }
    if (!ifBws.contains(_ifBw_Hz)) {
        errorMessage = "*IF bandwidth value not offered by VNA";
        return false;
    }

    // Power
    if (_startPower_dBm < vna.properties().minimumPower_dBm()) {
        errorMessage = "*Start power must be at least %1 dBm";
        errorMessage = errorMessage.arg(formatDouble(minPower_dBm, 1));
        return false;
    }
    if (_stopPower_dBm > vna.properties().maximumPower_dBm()) {
        errorMessage = "*Stop power must be at most %1 dBm";
        errorMessage = errorMessage.arg(formatDouble(maxPower_dBm, 1));
        return false;
    }
    if (_startPower_dBm >= _stopPower_dBm) {
        errorMessage = "*Start power must be less than stop power";
        return false;
    }
    if (_powerPoints < 1) {
        errorMessage = "*Power points must be at least 1";
        return false;
    }
    if (_powerPoints > vna.properties().maximumPoints()) {
        errorMessage = "*Power points must be at most %1";
        errorMessage = errorMessage.arg(maxPoints);
        return false;
    }
    if (_compressionLevel_dB <= 0) {
        errorMessage = "*Compression level must be greater than 0 dB";
        return false;
    }
//    if (_compressionLevel_dB > 10.0) {
//        // This seems Unreasonable...
//        errorMessage = "*Compression level is unusually high";
//        return false;
//    }

    // Misc
    if (!_channel) {
        errorMessage = "*Choose channel";
        return false;
    }
    if (!vna.isChannel(_channel)) {
        errorMessage = "*Channel does not exist";
        return false;
    }
    if (!_inputPort) {
        errorMessage = "*Choose input port";
        return false;
    }
    if (_inputPort > testPorts) {
        errorMessage = "*Input port must be at most %1";
        errorMessage = errorMessage.arg(testPorts);
        return false;
    }
    if (!_outputPort) {
        errorMessage = "*Choose output port";
        return false;
    }
    if (_outputPort > testPorts) {
        errorMessage = "*Output port must be at most %1";
        errorMessage = errorMessage.arg(testPorts);
        return false;
    }

    if (_outputPort == _inputPort) {
        errorMessage = "*Output port cannot be same as input port";
        return false;
    }

    // No errors
    errorMessage.clear();
    return true;
}

void MeasurementSettings::reset() {
    _startFreq_Hz = 0;
    _stopFreq_Hz = 1.0E12;
    _frequencyPoints = 101;
    _ifBw_Hz = 1.0E3;
    _startPower_dBm = -20.0;
    _stopPower_dBm = 0;
    _powerPoints = 81;
    _compressionLevel_dB = 1.0;
    _isGainExpansion = true;
    _isStopAtCompression = true;
    _isRfOffPostCondition = false;
    _channel = 1;
    _outputPort = 2;
    _inputPort = 1;
}

bool MeasurementSettings::printInfo(QString filename) const {
    if (!filename.endsWith(".txt", Qt::CaseInsensitive))
        filename += ".txt";

    QFile file(filename);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }
    QTextStream s(&file);
    printInfo(s);
    file.close();
    return true;
}
void MeasurementSettings::printInfo(QTextStream &s) const {
    s << "SETTINGS\n";
    s << "Start Frequency:     "
      << formatValue(_startFreq_Hz, 3, Units::Hertz)
      << "\n";
    s << "Stop Frequency:      "
      << formatValue(_stopFreq_Hz, 3, Units::Hertz)
      << "\n";
    s << "Points:              "
      << _frequencyPoints
      << "\n";
    s << "IF BW:               "
      << formatValue(_ifBw_Hz, 3, Units::Hertz)
      << "\n";
    s << "\n";

    s << "Start Power:         "
      << formatDouble(_startPower_dBm, 2) << " dBm"
      << "\n";
    s << "Stop Power:          "
      << formatDouble(_stopPower_dBm, 2) << " dBm"
      << "\n";
    s << "Points:              "
      << _powerPoints
      << "\n";
    s << "Compression Level:   "
      << formatDouble(_compressionLevel_dB, 2) << " dB"
      << "\n";
    s << "Gain Expansion:      "
      << (_isGainExpansion ? "Yes" : "No")
      << "\n";
    s << "Stop at Compression: "
      << (_isStopAtCompression ? "Yes" : "No")
      << "\n";
    s << "Post Condition:      "
      << (_isRfOffPostCondition ? "RF Off" : "None")
      << "\n";
    s << "\n";
    s.flush();
}
QString MeasurementSettings::printInfo() const {
    QString string;
    QTextStream stream(&string);
    printInfo(stream);
    return string;
}

QDataStream &operator>>(QDataStream &stream, MeasurementSettings &settings) {
    bool _bool;
    quint32 _uint;
    double _double;

    stream >> _double;
    settings.setStartFrequency(_double);
    stream >> _double;
    settings.setStopFrequency(_double);
    stream >> _uint;
    settings.setFrequencyPoints(_uint);
    stream >> _double;
    settings.setIfBw(_double);
    stream >> _double;
    settings.setStartPower(_double);
    stream >> _double;
    settings.setStopPower(_double);
    stream >> _uint;
    settings.setPowerPoints(_uint);
    stream >> _double;
    settings.setCompressionLevel(_double);
    stream >> _bool;
    settings.setGainExpansion(_bool);
    stream >> _bool;
    settings.setStopAtCompression(_bool);
    stream >> _bool;
    settings.setRfOffPostCondition(_bool);
    stream >> _uint;
    settings.setChannel(_uint);
    stream >> _uint;
    settings.setOutputPort(_uint);
    stream >> _uint;
    settings.setInputPort(_uint);

    return stream;
}
QDataStream &operator<<(QDataStream &stream, const MeasurementSettings &settings) {
    stream << settings.startFrequency_Hz()
           << settings.stopFrequency_Hz()
           << quint32(settings.frequencyPoints())
           << settings.ifBw_Hz()
           << settings.startPower_dBm()
           << settings.stopPower_dBm()
           << quint32(settings.powerPoints())
           << settings.compressionLevel_dB()
           << settings.isGainExpansion()
           << settings.isStopAtCompression()
           << settings.isRfOffPostCondition()
           << quint32(settings.channel())
           << quint32(settings.outputPort())
           << quint32(settings.inputPort());
    return stream;
}
