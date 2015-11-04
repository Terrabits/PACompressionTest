#include "MeasurementData.h"


//RsaToolbox
#include <General.h>
#include <Touchstone.h>
using namespace RsaToolbox;

// Qt
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

// QuaZip
#include <quazip.h>
#include <JlCompress.h>


MeasurementData::MeasurementData(QObject *parent) :
    QObject(parent)
{

}
MeasurementData::~MeasurementData()
{

}

bool MeasurementData::isValidSettings(Vna &vna) const {
    return _settings.isValid(vna);
}
bool MeasurementData::isValidSettings(Vna &vna, QString &errorMessage) const {
    return _settings.isValid(vna, errorMessage);
}
MeasurementSettings MeasurementData::settings() const {
    return _settings;
}
MeasurementSettings& MeasurementData::settings() {
    return _settings;
}
void MeasurementData::setSettings(const MeasurementSettings &settings) {
    _settings = settings;
}


void MeasurementData::processSettings() {
    frequencies_Hz = linearSpacing(_settings.startFrequency_Hz(),
                                   _settings.stopFrequency_Hz(),
                                   _settings.frequencyPoints());
    power_dBm = linearSpacing(_settings.startPower_dBm(),
                              _settings.stopPower_dBm(),
                              _settings.powerPoints());
    data.resize(_settings.powerPoints());
}

void MeasurementData::resizeToPoints() {
    resetData();
    data.resize(_settings.powerPoints());
}

void MeasurementData::calculateMetrics() {
    processReflectionCoefficients();
    processGain();
    findMaximumGain();
    processPowerOut();
    findCompressionPoints();
}

bool MeasurementData::open(QString filename) {
    QFile file(filename);
    if (!file.open(QFile::ReadOnly) || !file.isReadable())
        return false;

    QDataStream stream(&file);
    const bool success = open(stream);
    file.close();
    return success;
}
bool MeasurementData::save(QString filename) {
    QFile file(filename);
    if (!file.open(QFile::WriteOnly) || !file.isWritable())
        return false;

    QDataStream stream(&file);
    const bool success = save(stream);
    file.close();
    return success;
}
bool MeasurementData::exportToZip(QString filename) {
    if (filename.endsWith(".zip", Qt::CaseInsensitive))
        filename.chop(4);

    QString touchstone_folder = "Touchstone Files";
    QString nominal_gain_filename = "Nominal Gain.csv";
    QString compression_points_filename = "Compression points.csv";
    QFileInfo fileInfo(filename);
    QDir dir(fileInfo.absolutePath());
    if (!dir.mkdir(fileInfo.fileName())) {
        // Error
        return false;
    }
    dir.cd(fileInfo.fileName());

    // Touchstone
    dir.mkdir(touchstone_folder);
    exportTouchstone(dir.filePath(touchstone_folder));

    // Nominal Gain
    nominal_gain_filename = dir.filePath(nominal_gain_filename);
    exportReferenceGain(nominal_gain_filename);

    // Compression Point
    compression_points_filename = dir.filePath(compression_points_filename);
    exportCompressionPoints(compression_points_filename);

    JlCompress::compressDir(filename + ".zip", dir.path(), true);
    dir.removeRecursively();

    return true;
}

void MeasurementData::reset() {
    resetSettings();
    resetData();
    emit dataReset();
}
void MeasurementData::emitReset() {
    emit dataReset();
}

void MeasurementData::resetSettings() {
    applicationName.clear();
    applicationVersion.clear();
    timeStamp = QDateTime::currentDateTime();

    vnaMake.clear();
    vnaModel.clear();
    vnaSerialNo.clear();
    vnaFirmwareVersion.clear();

    _settings.reset();
}
void MeasurementData::resetData() {
    data.clear();

    s11_dB.clear();
    s22_dB.clear();
    gain_dB.clear();
    powerOut_dBm.clear();
    referenceGain_dB.clear();
    powerInAtCompression_dBm.clear();
    powerOutAtCompression_dBm.clear();
    compressionFrequencies_Hz.clear();
}

void MeasurementData::processReflectionCoefficients() {
    s11_dB = data[0].y_dB(1, 1);
    s22_dB = data[0].y_dB(2, 2);
}
void MeasurementData::processGain() {
    gain_dB.resize(_settings.frequencyPoints());
    for (uint iFreq = 0; iFreq < _settings.frequencyPoints(); iFreq++) {
        gain_dB[iFreq].resize(_settings.powerPoints());
        for (uint iPower = 0; iPower < _settings.powerPoints(); iPower++) {
            gain_dB[iFreq][iPower] = toDb(data[iPower].y()[iFreq][1][0]);
        }
    }
}
void MeasurementData::processPowerOut() {
    powerOut_dBm.resize(_settings.frequencyPoints());
    for (uint iFreq = 0; iFreq < _settings.frequencyPoints(); iFreq++) {
        powerOut_dBm[iFreq].resize(_settings.powerPoints());
        for (uint iPower = 0; iPower < _settings.powerPoints(); iPower++) {
            const double _gain_dB
                    = gain_dB[iFreq][iPower];
            const double _powerIn_dBm
                    = power_dBm[iPower];
            powerOut_dBm[iFreq][iPower]
                    = _powerIn_dBm + _gain_dB;
        }
    }
}
void MeasurementData::findMaximumGain() {
    referenceGainIndexes.resize(_settings.frequencyPoints());
    referenceGain_dB.resize(_settings.frequencyPoints());
    s_referenceGain.resize(_settings.frequencyPoints());
    for (uint i = 0; i < _settings.frequencyPoints(); i++) {
        if (_settings.isGainExpansion()) {
            int maxIndex;
            double maxValue;
            max(gain_dB[i], maxValue, maxIndex);
            referenceGainIndexes[i] = maxIndex;
            referenceGain_dB[i] = maxValue;
            s_referenceGain[i] = data[maxIndex].y()[i];
        }
        else {
            // Low power as reference:
            const int lowPowerIndex = 0;
            referenceGainIndexes[i] = lowPowerIndex;
            referenceGain_dB[i] = gain_dB[i][0];
            s_referenceGain[i] = data[lowPowerIndex].y()[i];
        }
    }
}
void MeasurementData::findCompressionPoints() {
    for (uint iFreq = 0; iFreq < _settings.frequencyPoints(); iFreq++) {
        uint iPower = referenceGainIndexes[iFreq];
        const double maxGain_dB = referenceGain_dB[iFreq];
        const double compressedGain_dB = maxGain_dB - _settings.compressionLevel_dB();
        double _gain_dB = gain_dB[iFreq][iPower];
        while (iPower < _settings.powerPoints()-1 && _gain_dB > compressedGain_dB) {
            iPower++;
            _gain_dB = gain_dB[iFreq][iPower];
        }

        if (_gain_dB == compressedGain_dB) {
            // Exact match
            powerInAtCompression_dBm << power_dBm[iPower];
            powerOutAtCompression_dBm << power_dBm[iPower] + _gain_dB;
            compressionFrequencies_Hz << frequencies_Hz[iFreq];
            s_compression.push_back(data[iPower].y()[iFreq]);
        }
        else if (_gain_dB < compressedGain_dB){
            // Apply linear interpolation
            powerInAtCompression_dBm << linearInterpolateX(power_dBm[iPower-1],
                                                            gain_dB[iFreq][iPower-1],
                                                            power_dBm[iPower],
                                                            _gain_dB,
                                                            compressedGain_dB);
            powerOutAtCompression_dBm << linearInterpolateY(
                    power_dBm[iPower-1],
                    power_dBm[iPower-1] + gain_dB[iFreq][iPower-1], //Pout[index-1]
                    power_dBm[iPower],
                    power_dBm[iPower] + _gain_dB, //Pout[index]
                    powerInAtCompression_dBm.last());
            compressionFrequencies_Hz << frequencies_Hz[iFreq];

            const double p_compressed = powerInAtCompression_dBm.last();
            ComplexMatrix2D s(2);
            for (int i = 0; i < 2; i++) {
                s[i].resize(2);
                for (int j = 0; j < 2; j++) {
                    s[i][j]
                            = linearInterpolateYMagPhase(
                                toMagnitude(power_dBm[iPower-1]),
                                data[iPower-1].y()[iFreq][i][j],
                                toMagnitude(power_dBm[iPower]),
                                data[iPower].y()[iFreq][i][j],
                                toMagnitude(p_compressed));
                }
            }
            s_compression.push_back(s);
        }
        else {
            qDebug() << iFreq << ": Missing compression point for frequency " << formatValue(frequencies_Hz[iFreq], 3, Units::Hertz);
        }
    }
}

// Export
void MeasurementData::exportTouchstone(QString path) {
    for (uint i = 0; i < _settings.powerPoints(); i++) {
        QString filename = path + "/"
                + "Pin " + QLocale().toString(power_dBm[i]) + " dBm.s2p";
        Touchstone::write(data[i], filename);
    }
}
void MeasurementData::exportReferenceGain(QString filename) {
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << "! (frequency, Nominal Gain) in Hz, dB" << endl;
    stream << "! @ Pin = "
           << formatValue(power_dBm[0], 2, Units::dBm)
            << " (minimum power)" << endl;
    stream.setFieldAlignment(QTextStream::AlignLeft);
    for (uint i = 0; i < _settings.frequencyPoints(); i++) {
        stream.setFieldWidth(18);
        stream.setRealNumberPrecision(18);
        stream << frequencies_Hz[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setRealNumberPrecision(5);
        stream << referenceGain_dB[i]
                  << endl;
    }
    stream.flush();
    file.close();
}
void MeasurementData::exportCompressionPoints(QString filename) {
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream << "! Compression points" << endl;
    stream << "! (frequency, Pin, Pout) in Hz, dBm, dBm" << endl;
    stream.setFieldAlignment(QTextStream::AlignLeft);
    int points = compressionFrequencies_Hz.size();
    for (int i = 0; i < points; i++) {
        stream.setFieldWidth(18);
        stream.setRealNumberPrecision(18);
        stream << compressionFrequencies_Hz[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setFieldWidth(12);
        stream.setRealNumberPrecision(6);
        stream << powerInAtCompression_dBm[i];
        stream.setFieldWidth(0);
        stream << ",  ";
        stream.setRealNumberPrecision(6);
        stream << powerOutAtCompression_dBm[i]
                  << endl;
    }
    stream.flush();
    file.close();
}

bool MeasurementData::open(QDataStream &stream) {
    try {
        stream >> applicationName
               >> applicationVersion
               >> timeStamp

               >> vnaMake
               >> vnaModel
               >> vnaSerialNo
               >> vnaFirmwareVersion

               >> _settings

               >> frequencies_Hz
               >> power_dBm
               >> s11_dB
               >> s22_dB
               >> gain_dB
               >> powerOut_dBm
               >> referenceGain_dB
               >> powerInAtCompression_dBm
               >> powerOutAtCompression_dBm
               >> compressionFrequencies_Hz;
        data.resize(_settings.powerPoints());
        for (uint i = 0; i < _settings.powerPoints(); i++) {
            stream >> data[i];
        }
        return true;
    }
    catch(void *) {
        return false;
    }
}
bool MeasurementData::save(QDataStream &stream) {
    try {
        stream << applicationName
               << applicationVersion
               << timeStamp

               << vnaMake
               << vnaModel
               << vnaSerialNo
               << vnaFirmwareVersion

               << _settings

               << frequencies_Hz
               << power_dBm
               << s11_dB
               << s22_dB
               << gain_dB
               << powerOut_dBm
               << referenceGain_dB
               << powerInAtCompression_dBm
               << powerOutAtCompression_dBm
               << compressionFrequencies_Hz;
        for (uint i = 0; i < _settings.powerPoints(); i++) {
            stream << data[i];
        }
        return true;
    }
    catch(void *) {
        return false;
    }
}
