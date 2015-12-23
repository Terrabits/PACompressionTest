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
void MeasurementData::setVnaInfo(Vna &vna) {
    vna.printInfo(_vnaInfo);
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
    for (int i = 0; i < _pin_dBm.size(); i++) {
        const int fIndex = _data[i].x().indexOf(frequency_Hz);
        if (fIndex == -1) {
            continue;
        }

        measuredPin_dBm << _measuredPin_dBm[i][fIndex];
        sParameter.push_back(_data[i].y()[fIndex][outputPort-1][inputPort-1]);
    }
    if (measuredPin_dBm.isEmpty()) {
        return false;
    }

    // Reinterpolate using measured Pin (a1)
    pin_dBm = linearSpacing(measuredPin_dBm.first(), measuredPin_dBm.last(), measuredPin_dBm.size());
    sParameter = linearInterpolateMagPhase(measuredPin_dBm, sParameter, pin_dBm);
    return true;
}
bool MeasurementData::sParameterVsPout(double frequency_Hz, uint outputPort, uint inputPort, QRowVector &pout_dBm, ComplexRowVector &sParameter) {
    pout_dBm.clear();
    sParameter.clear();

    QRowVector pin_dBm;
    ComplexRowVector gain;
    if (!sParameterVsPin(frequency_Hz, 2, 1, pin_dBm, gain))
        return false;

    QRowVector _pout_dBm = add(pin_dBm, toDb(gain));
    if (outputPort == 2 && inputPort == 1) {
        sParameter = gain;
    }
    else {
        if (!sParameterVsPin(frequency_Hz, outputPort, inputPort, pin_dBm, sParameter))
            return false;
    }

    // Reinterpolate onto square grid vs Pout
    pout_dBm = linearSpacing(_pout_dBm.first(), _pout_dBm.last(), _pout_dBm.size());
    sParameter = linearInterpolateMagPhase(_pout_dBm, sParameter, pout_dBm);
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

QVector<QRowVector> &MeasurementData::measuredPin_dBm() {
    return _measuredPin_dBm;
}

QVector<NetworkData> &MeasurementData::data() {
    return _data;
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

    _data.clear();
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
    stream >> _powerOutAtMaxGain_dBm;

    stream >> _powerInAtCompression_dBm;
    stream >> _gainAtCompression_dB;
    stream >> _powerOutAtCompression_dBm;

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
    stream << _powerOutAtMaxGain_dBm;

    stream << _powerInAtCompression_dBm;
    stream << _gainAtCompression_dB;
    stream << _powerOutAtCompression_dBm;

    stream << _data;

    file.close();
    return true;
}

bool MeasurementData::exportToZip(QString filename) {
    if (filename.endsWith(".zip", Qt::CaseInsensitive))
        filename.chop(4);

    QString s2pFolder = "Touchstone Files";
    QString simpleCsvFile = "Compression.csv";
    QString completeCsvFile = "Data.csv";
    QString infoFile = "Settings.txt";
    QFileInfo fileInfo(filename);
    QDir dir(fileInfo.absolutePath());

    if (!dir.mkdir(fileInfo.fileName())) {
        return false;
    }
    dir.cd(fileInfo.fileName());

    qDebug() << "info";
    infoFile = dir.filePath(infoFile);
    if (!exportInfo(infoFile)) {
        dir.removeRecursively();
        return false;
    }

    qDebug() << "simple csv";
    simpleCsvFile = dir.filePath(simpleCsvFile);
    if (!exportSimpleCsv(simpleCsvFile)) {
        dir.removeRecursively();
        return false;
    }

    qDebug() << "complete csv";
    completeCsvFile = dir.filePath(completeCsvFile);
    if (!exportCompleteCsv(completeCsvFile)) {
        qDebug() << "complete csv failed";
        dir.removeRecursively();
        return false;
    }

    qDebug() << "touchstone";
    if (!dir.mkdir(s2pFolder)) {
        dir.removeRecursively();
        return false;
    }
    s2pFolder = dir.filePath(s2pFolder);
    if (!exportTouchstone(s2pFolder)) {
        dir.removeRecursively();
        return false;
    }

    filename += ".zip";
    bool success = JlCompress::compressDir(filename, dir.path(), true);
    dir.removeRecursively();
    qDebug() << "/Export";
    return success;
}
bool MeasurementData::exportInfo(QString path) {
    Log log(path, _appName, _appVersion);
    log.printHeader();
    log.print(_settings.printInfo());
    log.print(_vnaInfo);
    return true;
}
bool MeasurementData::exportSimpleCsv(QString path) {
    if (!path.endsWith(".csv", Qt::CaseInsensitive))
        path += ".csv";

    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return false;

    const int FIELD_WIDTH = 25;
    QTextStream s(&file);
    s.setFieldAlignment(QTextStream::AlignLeft);
    s.setFieldWidth(FIELD_WIDTH);

    s.setRealNumberNotation(QTextStream::ScientificNotation);
    s.setRealNumberPrecision(15);

    s << "Frequency_Hz";
    s << "Pin[Max Gain]_dBm";
    s << "Max Gain_dB";
    s << "Pout[Max]_dBm";
    s << "Pin[Comp]_dBm";
    s << "Gain[Comp]_dB";
    s << "Pout[Comp]_dBm";

    s.setFieldWidth(0);
    s << "\n";
    s.setFieldWidth(FIELD_WIDTH);

    for (int i = 0; i < _frequencies_Hz.size(); i++) {
        s << _frequencies_Hz[i];
        s << _powerInAtMaxGain_dBm[i];
        s << _maxGain_dB[i];
        s << _powerOutAtMaxGain_dBm[i];
        s << _powerInAtCompression_dBm[i];
        s << _gainAtCompression_dB[i];
        s << _powerOutAtCompression_dBm[i];

        s.setFieldWidth(0);
        s << "\n";
        s.setFieldWidth(FIELD_WIDTH);
    }

    s.flush();
    file.close();
    return true;
}
bool MeasurementData::exportCompleteCsv(QString path) {
    if (!path.endsWith(".csv", Qt::CaseInsensitive))
        path += ".csv";

    QFile file(path);
    if (!file.open(QFile::WriteOnly))
        return false;

    const int FIELD_WIDTH = 25;
    QTextStream s(&file);
    s.setFieldAlignment(QTextStream::AlignLeft);
    s.setFieldWidth(FIELD_WIDTH);

    s.setRealNumberNotation(QTextStream::ScientificNotation);
    s.setRealNumberPrecision(15);

    s << "Frequency_Hz";
    s << "Pin_dBm";
    s << "Pout_dBm";
    s << "S11_dB";
    s << "S11_deg";
    s << "S21_dB";
    s << "S21_deg";
    s << "S12_dB";
    s << "S12_deg";
    s << "S22_dB";
    s << "S22_deg";

    s.setFieldWidth(0);
    s << "\n";
    s.setFieldWidth(FIELD_WIDTH);

    for (int iFreq = 0; iFreq < _frequencies_Hz.size(); iFreq++) {
        const double freq_Hz = _frequencies_Hz[iFreq];

        for (int iPower = 0; iPower < _pin_dBm.size(); iPower++) {
            const int iCurrentFreq = _data[iPower].x().indexOf(freq_Hz);
            if (iCurrentFreq == -1)
                continue;

            const ComplexDouble s11 = _data[iPower].y()[iCurrentFreq][0][0];
            const ComplexDouble s21 = _data[iPower].y()[iCurrentFreq][1][0];
            const ComplexDouble s12 = _data[iPower].y()[iCurrentFreq][0][1];
            const ComplexDouble s22 = _data[iPower].y()[iCurrentFreq][1][1];

            s << freq_Hz;
            s << _measuredPin_dBm[iPower][iCurrentFreq];
            s << _measuredPin_dBm[iPower][iCurrentFreq] + toDb(s21);
            s << toDb(s11);
            s << angle_deg(s11);
            s << toDb(s21);
            s << angle_deg(s21);
            s << toDb(s12);
            s << angle_deg(s12);
            s << toDb(s22);
            s << angle_deg(s22);

            s.setFieldWidth(0);
            s << "\n";
            s.setFieldWidth(FIELD_WIDTH);
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
