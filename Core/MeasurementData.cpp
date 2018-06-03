#include "MeasurementData.h"


//RsaToolbox
#include <General.h>
#include <Touchstone.h>
#include <Log.h>
using namespace RsaToolbox;

// Qt
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDataStream>

// QuaZip
#include <quazip.h>
#include <quazipfile.h>
#include <JlCompress.h>


MeasurementData::MeasurementData() :
    _timeStamp(QDateTime::currentDateTime())
{

}
MeasurementData::~MeasurementData()
{

}

void MeasurementData::setAppInfo(const QString &name, const QString &version) {
    _appName = name;
    _appVersion = version;
}
void MeasurementData::setTimeToNow() {
    _timeStamp = QDateTime::currentDateTime();
}

MeasurementSettings MeasurementData::settings() const {
    return _settings;
}
void MeasurementData::setSettings(const MeasurementSettings &settings) {
    _settings = settings;
}

uint MeasurementData::frequencyPoints() const {
    return _frequencies_Hz.size();
}
QRowVector &MeasurementData::frequencies_Hz() {
    return _frequencies_Hz;
}
uint  MeasurementData::powerPoints() const {
    return _pin_dBm.size();
}
QRowVector &MeasurementData::pin_dBm() {
    return _pin_dBm;
}

QRowVector &MeasurementData::powerInAtMaxGain_dBm() {
    return _powerInAtMaxGain_dBm;
}
QRowVector &MeasurementData::maxGain_dB() {
    return _maxGain_dB;
}
ComplexMatrix3D &MeasurementData::sParametersAtMaxGain() {
    return _sParametersAtMaxGain;
}
QRowVector &MeasurementData::powerOutAtMaxGain_dBm() {
    return _powerOutAtMaxGain_dBm;
}
QRowVector &MeasurementData::powerInAtCompression_dBm() {
    return _powerInAtCompression_dBm;
}
QRowVector &MeasurementData::gainAtCompression_dB() {
    return _gainAtCompression_dB;
}
ComplexMatrix3D &MeasurementData::sParametersAtCompression() {
    return _sParametersAtCompression;
}
QRowVector &MeasurementData::powerOutAtCompression_dBm() {
    return _powerOutAtCompression_dBm;
}
QVector<QRowVector> &MeasurementData::dcPowerAtCompression_W() {
    return _dcPowerAtCompression_W;
}
QVector<QRowVector> &MeasurementData::dcCurrentAtCompression_A() {
    return _dcCurrentAtCompression_A;
}

ComplexRowVector MeasurementData::sParameterAtCompression(uint outputPort, uint inputPort) {
    ComplexRowVector _result(frequencyPoints());
    for (uint i = 0; i < frequencyPoints(); i++) {
        _result[i] = _sParametersAtCompression[i][outputPort-1][inputPort-1];
    }
    return _result;
}
ComplexRowVector MeasurementData::sParameterAtMaxGain(uint outputPort, uint inputPort) {
    ComplexRowVector _result(frequencyPoints());
    for (uint i = 0; i < frequencyPoints(); i++) {
        _result[i] = _sParametersAtMaxGain[i][outputPort-1][inputPort-1];
    }
    return _result;
}

bool MeasurementData::sParameterVsPin(double frequency_Hz, uint outputPort, uint inputPort, QRowVector &pin_dBm, ComplexRowVector &sParameter) {
    pin_dBm.clear();
    sParameter.clear();

    QRowVector measuredPin_dBm;
    if (!sParameterVsPin_uninterpolated(frequency_Hz, outputPort, inputPort, measuredPin_dBm, sParameter))
        return false;

    // Reinterpolate using measured Pin (a1)
    const double start_dBm = measuredPin_dBm.first();
    const double stop_dBm = measuredPin_dBm.last();

    const uint factor = 10;
    uint points = measuredPin_dBm.size();
    points = factor * points - factor + 1;

    pin_dBm = linearSpacing(start_dBm, stop_dBm, points);
    sParameter = linearInterpolateMagPhase(measuredPin_dBm, sParameter, pin_dBm);
    return true;
}
bool MeasurementData::sParameterVsPout(double frequency_Hz, uint outputPort, uint inputPort, QRowVector &pout_dBm, ComplexRowVector &sParameter) {
    pout_dBm.clear();
    sParameter.clear();

    QRowVector pin_dBm;
    ComplexRowVector s21;
    if (!sParameterVsPin_uninterpolated(frequency_Hz, 2, 1, pin_dBm, s21))
        return false;

    QRowVector initialPout_dBm = add(pin_dBm, toDb(s21));
    if (outputPort == 2 && inputPort == 1) {
        sParameter = s21;
    }
    else {
        if (!sParameterVsPin_uninterpolated(frequency_Hz, outputPort, inputPort, pin_dBm, sParameter))
            return false;
    }

    // Reinterpolate onto square grid vs Pout
    const uint factor = 10;
    uint points = initialPout_dBm.size();
    points = factor * points - factor + 1;
    const double start_dBm = initialPout_dBm.first();
    const double stop_dBm = max(initialPout_dBm);
    pout_dBm = linearSpacing(start_dBm, stop_dBm, points);
    sParameter = linearInterpolateMagPhase(initialPout_dBm, sParameter, pout_dBm);

    return true;
}
bool MeasurementData::sParameterVsFrequencyAtPin(double pin_dBm, uint outputPort, uint inputPort, QRowVector &frequencies_Hz, ComplexRowVector &sParameter) {
    frequencies_Hz.clear();
    sParameter.clear();
    const int i = _pin_dBm.indexOf(pin_dBm);
    if (i == -1) {
        return false;
    }

    frequencies_Hz = _data[i].x();
    sParameter = _data[i].y(outputPort, inputPort);
    return true;
}

bool MeasurementData::poutVsFrequency(double pin_dBm, QRowVector &frequencies_Hz, QRowVector &pout_dBm) {
    frequencies_Hz.clear();
    pout_dBm.clear();

    const int i = _pin_dBm.indexOf(pin_dBm);
    if (i == -1) {
        return false;
    }
    const QRowVector measuredPin_dBm = _measuredPin_dBm[i];

    ComplexRowVector gain;
    if (!sParameterVsFrequencyAtPin(pin_dBm, 2, 1, frequencies_Hz, gain)) {
        frequencies_Hz.clear();
        return false;
    }

    pout_dBm = add(measuredPin_dBm, toDb(gain));
    return true;
}
bool MeasurementData::poutVsPin(double frequency_Hz, QRowVector &pin_dBm, QRowVector &pout_dBm) {
    pin_dBm.clear();
    pout_dBm.clear();

    ComplexRowVector gain;
    if (!sParameterVsPin(frequency_Hz, 2, 1, pin_dBm, gain)) {
        pin_dBm.clear();
        return false;
    }

    pout_dBm = add(pin_dBm, toDb(gain));
    return true;
}

bool MeasurementData::amPmVsPin(double frequency_Hz, QRowVector &pin_dBm, QRowVector &amPm_deg) {
    pin_dBm.clear();
    amPm_deg.clear();

    ComplexRowVector s21;
    if (!sParameterVsPin(frequency_Hz, 2, 1, pin_dBm, s21))
        return false;

    amPm_deg = angle_deg(s21);
    amPm_deg = unwrap(amPm_deg, 360.0);
    amPm_deg = subtract(amPm_deg, amPm_deg.first());
    return true;
}
bool MeasurementData::amPmVsPout(double frequency_Hz, QRowVector &pout_dBm, QRowVector &amPm_deg) {
    pout_dBm.clear();
    amPm_deg.clear();

    ComplexRowVector s21;
    if (!sParameterVsPout(frequency_Hz, 2, 1, pout_dBm, s21))
        return false;

    amPm_deg = angle_deg(s21);
    amPm_deg = unwrap(amPm_deg, 360.0);
    amPm_deg = subtract(amPm_deg, amPm_deg.first());
    return true;
}

QVector<QRowVector> &MeasurementData::measuredPin_dBm() {
    return _measuredPin_dBm;
}

QVector<NetworkData> &MeasurementData::data() {
    return _data;
}
QVector<QVector<dmm::StageResult>> &MeasurementData::dmmData() {
    return _dmmData;
}
void MeasurementData::clearAllData() {
    _frequencies_Hz.clear();
    _pin_dBm.clear();

    _powerInAtMaxGain_dBm.clear();
    _maxGain_dB.clear();
    _powerOutAtMaxGain_dBm.clear();

    _powerInAtCompression_dBm.clear();
    _gainAtCompression_dB.clear();
    _powerOutAtCompression_dBm.clear();
    _dcPowerAtCompression_W.clear();

    _data.clear();
    _dmmData.clear();
}

bool MeasurementData::open(QString filename) {
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return false;

    QDataStream stream(&file);
    stream >> _appName;
    stream >> _appVersion;
    stream >> _timeStamp;
    stream >> _vnaInfo;
    stream >> _settings;

    stream >> _frequencies_Hz;
    stream >> _pin_dBm;

    stream >> _powerInAtMaxGain_dBm;
    stream >> _maxGain_dB;
    stream >> _sParametersAtMaxGain;
    stream >> _powerOutAtMaxGain_dBm;

    stream >> _powerInAtCompression_dBm;
    stream >> _gainAtCompression_dB;
    stream >> _sParametersAtCompression;
    stream >> _powerOutAtCompression_dBm;

    stream >> _measuredPin_dBm;
    stream >> _data;

    file.close();
    return true;
}
bool MeasurementData::save(QString filename) {
    QFile file(filename);
    if (!file.open(QFile::WriteOnly))
        return false;

    QDataStream stream(&file);
    stream << _appName;
    stream << _appVersion;
    stream << _timeStamp;
    stream << _vnaInfo;
    stream << _settings;

    stream << _frequencies_Hz;
    stream << _pin_dBm;

    stream << _powerInAtMaxGain_dBm;
    stream << _maxGain_dB;
    stream << _sParametersAtMaxGain;
    stream << _powerOutAtMaxGain_dBm;

    stream << _powerInAtCompression_dBm;
    stream << _gainAtCompression_dB;
    stream << _sParametersAtCompression;
    stream << _powerOutAtCompression_dBm;

    stream << _measuredPin_dBm;
    stream << _data;

    file.close();
    return true;
}

bool MeasurementData::sParameterVsPin_uninterpolated(double frequency_Hz, uint outputPort, uint inputPort, QRowVector &pin_dBm, ComplexRowVector &sParameter) {
    pin_dBm.clear();
    sParameter.clear();

    for (int i = 0; i < _pin_dBm.size(); i++) {
        const int fIndex = _data[i].x().indexOf(frequency_Hz);
        if (fIndex == -1) {
            continue;
        }

        pin_dBm << _measuredPin_dBm[i][fIndex];
        sParameter.push_back(_data[i].y()[fIndex][outputPort-1][inputPort-1]);
    }

    if (pin_dBm.isEmpty())
        return false;
    else
        return true;
}

// Header
void MeasurementData::createExportFileHeader(Vna &vna) {
    _header.clear();

    QTextStream stream(&_header);
    stream << generateApplicationHeader();
    stream << generateCopyright();
    stream << "!" << endl;
    stream << generateTimeStamp();
    stream << "!" << endl;
    stream << generateVnaInfo(vna);
    stream << "!" << endl;
    stream << generatePulsedRfInfo(vna);
    stream << "!" << endl;
    stream << generateSettingsSummary();
    stream << "!" << endl;
    stream.flush();
}
QString MeasurementData::generateApplicationHeader() const {
    QString header;
    if (_appName.isEmpty() || _appVersion.isEmpty())
        return header;

    QTextStream stream(&header);
    stream << "! " << _appName << " Version " << _appVersion << endl;
    stream.flush();

    return header;
}
QString MeasurementData::generateCopyright() const {
    QString header;

    QString date(__DATE__);
    date.remove(0, date.size()-4);

    QTextStream stream(&header);
    stream << "! (C) " + date + " Rohde & Schwarz North America" << endl;
    stream.flush();

    return header;
}
QString MeasurementData::generateTimeStamp() const {
    QString header;
    QTextStream stream(&header);
    stream << "! " << _timeStamp.toString() << endl;
    stream.flush();
    return header;
}
QString MeasurementData::generateVnaInfo(Vna &vna) const {
    QString vnaInfo;
    QTextStream stream(&vnaInfo);

    stream << "! VNA INSTRUMENT INFO" << endl;
    stream << "! Connection:       " << toString(vna.connectionType()) << endl;
    stream << "! Address:          " << vna.address() << endl;
    stream << "! Make:             Rohde & Schwarz" << endl;
    stream << "! Model:            " << toString(vna.properties().model()) << endl;
    stream << "! Serial No:        " << vna.properties().serialNumber() << endl;
    stream << "! Firmware Version: " << vna.properties().firmwareVersion() << endl;
    stream << "! Min Frequency:    " << formatValue(vna.properties().minimumFrequency_Hz(), 1, Units::Hertz) << endl;
    stream << "! Max Frequency:    " << formatValue(vna.properties().maximumFrequency_Hz(), 1, Units::Hertz) << endl;
    stream << "! Number of Ports:  " << vna.properties().physicalPorts() << endl;
    if (!vna.optionsString().isEmpty()) {
        stream << "! Options:" << endl << "!     ";
        stream << toString(vna.properties().optionsList(), "\n!     ");
        stream << endl;
    }


    stream.flush();
    return vnaInfo;
}
QString MeasurementData::generatePulsedRfInfo(Vna &vna) const {
    QString pulsedRfInfo;
    QTextStream stream(&pulsedRfInfo);

    stream << "! PULSED RF? ";
    const uint c = _settings.channel();
    const uint inputPort = _settings.inputPort();
    bool isPulsedRf = vna.isExtensionUnit() && vna.channel(c).extensionUnit().isPulseModulatorOn(inputPort);
    if (isPulsedRf) {
        stream << "Yes" << endl;
        stream << "! Pulse Width:   " << formatValue(vna.channel(c).pulseGenerator().pulseWidth_s(), 3, Units::Seconds) << endl;
        stream << "! Period:        " << formatValue(vna.channel(c).pulseGenerator().period_s(), 3, Units::Seconds) << endl;
        stream << "! Trigger delay: " << formatValue(vna.channel(c).trigger().delay_s(), 3, Units::Seconds) << endl;
        stream << "!" << endl;
    }
    else {
        stream << "No" << endl;
    }

    stream.flush();
    return pulsedRfInfo;
}
QString MeasurementData::generateSettingsSummary() const {
    QString summary;

    QTextStream stream(&summary);
    stream << "! SETTINGS" << endl;
    stream << "! Start Frequency:        " << formatValue(_settings.startFrequency_Hz(), 3, Units::Hertz) << endl;
    stream << "! Stop Frequency:         " << formatValue(_settings.stopFrequency_Hz(), 3, Units::Hertz) << endl;
    stream << "! Frequency Points:       " << _settings.frequencyPoints() << endl;
    stream << "! IF BW:                  " << formatValue(_settings.ifBw_Hz(), 3, Units::Hertz) << endl;
    stream << "!" << endl;
    stream << "! Start Power:            " << formatValue(_settings.startPower_dBm(), 3, Units::dBm) << endl;
    stream << "! Stop Power:             " << formatValue(_settings.stopPower_dBm(), 3, Units::dBm) << endl;
    stream << "! Power Points:           " << _settings.powerPoints() << endl;
    stream << "! Compression value:      " << formatValue(_settings.compressionLevel_dB(), 3, Units::dB) << endl;
    stream << "! Include gain expansion? ";
    if (_settings.isGainExpansion())
        stream << "Yes" << endl;
    else
        stream << "No" << endl;
    stream << "! Stop at compression?    ";
    if (_settings.isStopAtCompression())
        stream << "Yes" << endl;
    else
        stream << "No" << endl;
    stream << "! Post Condition:         ";
    if (_settings.isRfOffPostCondition())
        stream << "RF Off" << endl;
    else
        stream << "None" << endl;
    stream << "!" << endl;

    stream << "! Channel:                " << _settings.channel() << endl;
    stream << "! Input port:             " << _settings.inputPort() << endl;
    stream << "! Output port:            " << _settings.outputPort() << endl;
    stream << "!" << endl;

    stream.flush();
    return summary;
}

bool MeasurementData::exportToZip(QString filename) {
    if (filename.endsWith(".zip", Qt::CaseInsensitive))
        filename.chop(4);

    QString touchstoneFolder = "Touchstone Files";
    QString compressionCsvFile = "Compression.csv";
    QString dataCsvFile = "Data.csv";
    QFileInfo fileInfo(filename);
    QDir dir(fileInfo.absolutePath());

    if (!dir.mkdir(fileInfo.fileName())) {
        return false;
    }
    dir.cd(fileInfo.fileName());

    compressionCsvFile = dir.filePath(compressionCsvFile);
    if (!exportCompressionCsv(compressionCsvFile)) {
        dir.removeRecursively();
        return false;
    }

    dataCsvFile = dir.filePath(dataCsvFile);
    if (!exportDataCsv(dataCsvFile)) {
        dir.removeRecursively();
        return false;
    }

    if (!dir.mkdir(touchstoneFolder)) {
        dir.removeRecursively();
        return false;
    }
    touchstoneFolder = dir.filePath(touchstoneFolder);
    if (!exportTouchstone(touchstoneFolder)) {
        dir.removeRecursively();
        return false;
    }

    filename += ".zip";
    bool success = JlCompress::compressDir(filename, dir.path(), true);
    dir.removeRecursively();
    return success;
}
bool MeasurementData::exportCompressionCsv(QString path) {
    const int FIELD_WIDTH = 25;

    if (!path.endsWith(".csv", Qt::CaseInsensitive))
        path += ".csv";

    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return false;

    QTextStream s(&file);
    s << _header;

    s.setFieldAlignment(QTextStream::AlignLeft);
    s.setFieldWidth(FIELD_WIDTH);

    s << "Frequency_Hz,";
    s << "Pin[Max Gain]_dBm,";
    s << "Max Gain_dB,";
    s << "Pout[Max]_dBm,";
    s << "Pin[Comp]_dBm,";
    s << "Gain[Comp]_dB,";
    s.setFieldWidth(0);
    s << "Pout[Comp]_dBm";
    for (int i = 0; i < _frequencies_Hz.size(); i++) {
        s << "\n";
        s.setFieldWidth(FIELD_WIDTH);

        s << toScientificNotationWithComma(_frequencies_Hz[i]);
        s << toScientificNotationWithComma(_powerInAtMaxGain_dBm[i]);
        s << toScientificNotationWithComma(_maxGain_dB[i]);
        s << toScientificNotationWithComma(_powerOutAtMaxGain_dBm[i]);
        s << toScientificNotationWithComma(_powerInAtCompression_dBm[i]);
        s << toScientificNotationWithComma(_gainAtCompression_dB[i]);
        s.setFieldWidth(0);
        s << toScientificNotation(_powerOutAtCompression_dBm[i]);
    }

    s.flush();
    file.close();
    return true;
}
bool MeasurementData::exportDataCsv(QString path) {
    const int FIELD_WIDTH = 26;

    if (!path.endsWith(".csv", Qt::CaseInsensitive))
        path += ".csv";

    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }

    QTextStream s(&file);
    s << _header;

    s.setFieldAlignment(QTextStream::AlignLeft);
    s.setFieldWidth(FIELD_WIDTH);

    s << "Frequency_Hz,";
    s << "Pin_dBm,";
    s << "Pin_deg,";
    s << "Pout_dBm,";
    s << "Pout_deg,";
    s << "AMPM_deg,";
    s << "S11_dB,";
    s << "S11_deg,";
    s << "S21_dB,";
    s << "S21_deg,";
    s << "S12_dB,";
    s << "S12_deg,";
    s << "S22_dB,";
    s.setFieldWidth(0);
    s << "S22_deg";

    for (int iFreq = 0; iFreq < _frequencies_Hz.size(); iFreq++) {
        const double freq_Hz = _frequencies_Hz[iFreq];

        // Gather relevant values for freq_Hz
        QRowVector pin_dBm;
        ComplexRowVector s11, s21, s12, s22;
        for (int iPower = 0; iPower < _measuredPin_dBm.size(); iPower++) {
            const int iCurrentFreq = _data[iPower].x().indexOf(freq_Hz);
            if (iCurrentFreq == -1) {
                break; // instead of continue?
            }

            pin_dBm.append(_measuredPin_dBm[iPower][iCurrentFreq]);
            s11.push_back(_data[iPower].y()[iCurrentFreq][0][0]);
            s21.push_back(_data[iPower].y()[iCurrentFreq][1][0]);
            s12.push_back(_data[iPower].y()[iCurrentFreq][0][1]);
            s22.push_back(_data[iPower].y()[iCurrentFreq][1][1]);
        }

        // Calculate pout, AMPM
        QRowVector pout_dBm, pout_deg, amPm_deg;
        pout_dBm = add(pin_dBm, toDb(s21));
        pout_deg = angle_deg(s21);
        amPm_deg = unwrap(pout_deg, 360);
        amPm_deg = subtract(amPm_deg, amPm_deg.first());

        // Write values for freq_Hz to CSV
        for (int iPower = 0; iPower < pin_dBm.size(); iPower++) {
            s << "\n";
            s.setFieldWidth(FIELD_WIDTH);

            s << toScientificNotationWithComma(freq_Hz);
            s << toScientificNotationWithComma(pin_dBm[iPower]);
            s << toScientificNotationWithComma(0); // Pin_deg (0 by def)
            s << toScientificNotationWithComma(pout_dBm[iPower]);
            s << toScientificNotationWithComma(pout_deg[iPower]); // Pout_deg ( = s21_deg by def)
            s << toScientificNotationWithComma(amPm_deg[iPower]);
            s << toScientificNotationWithComma(toDb(s11[iPower]));
            s << toScientificNotationWithComma(angle_deg(s11[iPower]));
            s << toScientificNotationWithComma(toDb(s21[iPower]));
            s << toScientificNotationWithComma(angle_deg(s21[iPower]));
            s << toScientificNotationWithComma(toDb(s12)[iPower]);
            s << toScientificNotationWithComma(angle_deg(s12[iPower]));
            s << toScientificNotationWithComma(toDb(s22[iPower]));
            s.setFieldWidth(0);
            s << toScientificNotation(angle_deg(s22[iPower]));
        }
    }

    s.flush();
    file.close();
    return true;
}
bool MeasurementData::exportTouchstone(QString path) {
    QDir _path(path);
    for (int i = 0; i < _data.size(); i++) {
        QString filename = "Pin %1 dBm.s2p";
        filename = filename.arg(formatDouble(_pin_dBm[i], 2));
        filename = _path.filePath(filename);
        if (!Touchstone::write(_data[i], filename)) {
            return false;
        }
    }
    return true;
}

QString MeasurementData::toScientificNotation(const double value) {
    QString result;
    QTextStream stream(&result);
    stream.setRealNumberNotation(QTextStream::ScientificNotation);
    stream.setRealNumberPrecision(15);

    stream << value;
    stream.flush();
    return result;
}
QString MeasurementData::toScientificNotationWithComma(const double value) {
    QString result;
    QTextStream stream(&result);
    stream.setRealNumberNotation(QTextStream::ScientificNotation);
    stream.setRealNumberPrecision(15);

    stream << value << ",";
    stream.flush();
    return result;
}
