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
    QObject(parent),
    inputPort(0), outputPort(0),
    startPower_dBm(0), stopPower_dBm(0),
    powerPoints(0),
    startFreq_Hz(0), stopFreq_Hz(0),
    frequencyPoints(0),
    ifBw_Hz(0),
    compressionLevel_dB(0),
    sourceAttenuation_dB(0), receiverAttenuation_dB(0)
{

}
MeasurementData::~MeasurementData()
{

}

void MeasurementData::processSettings() {
    frequencies_Hz = linearSpacing(startFreq_Hz, stopFreq_Hz, frequencyPoints);
    power_dBm = linearSpacing(startPower_dBm, stopPower_dBm, powerPoints);
    data.resize(powerPoints);
}

void MeasurementData::resizeToPoints() {
    resetData();
    data.resize(powerPoints);
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
    inputPort = 0;
    outputPort = 0;
    startPower_dBm = 0;
    stopPower_dBm = 0;
    powerPoints = 0;
    startFreq_Hz = 0;
    stopFreq_Hz = 0;
    frequencyPoints = 0;
    ifBw_Hz = 0;
    compressionLevel_dB = 0;
    sourceAttenuation_dB = 0;
    receiverAttenuation_dB = 0;

    frequencies_Hz.clear();
    power_dBm.clear();
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
    gain_dB.resize(frequencyPoints);
    for (uint iFreq = 0; iFreq < frequencyPoints; iFreq++) {
        gain_dB[iFreq].resize(powerPoints);
        for (uint iPower = 0; iPower < powerPoints; iPower++) {
            gain_dB[iFreq][iPower] = toDb(data[iPower].y()[iFreq][1][0]);
        }
    }
}
void MeasurementData::processPowerOut() {
    powerOut_dBm.resize(frequencyPoints);
    for (uint iFreq = 0; iFreq < frequencyPoints; iFreq++) {
        powerOut_dBm[iFreq].resize(powerPoints);
        for (uint iPower = 0; iPower < powerPoints; iPower++) {
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
    referenceGainIndexes.resize(frequencyPoints);
    referenceGain_dB.resize(frequencyPoints);
    s_referenceGain.resize(frequencyPoints);
    for (uint i = 0; i < frequencyPoints; i++) {
        // Edit to accomodate gain expansion:
        //
        // int j;
        // referenceGain_dB[i] = max(gain_dB[i], referenceGain_dB[i], j);
        //

        // Low power as reference:
        int j = 0;

        referenceGainIndexes[i] = j;
        referenceGain_dB[i] = gain_dB[i][0];
        s_referenceGain[i] = data[j].y()[i];
    }
}
void MeasurementData::findCompressionPoints() {
    for (uint iFreq = 0; iFreq < frequencyPoints; iFreq++) {
        uint iPower = referenceGainIndexes[iFreq];
        const double maxGain_dB = referenceGain_dB[iFreq];
        const double compressedGain_dB = maxGain_dB - compressionLevel_dB;
        double _gain_dB = gain_dB[iFreq][iPower];
        while (iPower < powerPoints-1 && _gain_dB > compressedGain_dB) {
            iPower++;
            _gain_dB = gain_dB[iFreq][iPower];
        }

        qDebug() << iFreq << " ----------";
        qDebug() << "Max gain: " << maxGain_dB << " dB";
        qDebug() << "Compressed gain: " << compressedGain_dB << " dB";
        qDebug() << "iPower: " << iPower;
        qDebug() << "Pin: " << power_dBm[iPower] << " dBm";
        qDebug() << "Gain: " << _gain_dB << " dB";
        qDebug() << "Pin (prev): " << power_dBm[iPower-1] << " dBm";
        qDebug() << "Gain (prev): " << gain_dB[iFreq][iPower-1] << " dB";

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
            qDebug() << "Pin (compression): " << powerInAtCompression_dBm.last() << " dBm";
            powerOutAtCompression_dBm << linearInterpolateY(
                    power_dBm[iPower-1],
                    power_dBm[iPower-1] + gain_dB[iFreq][iPower-1], //Pout[index-1]
                    power_dBm[iPower],
                    power_dBm[iPower] + _gain_dB, //Pout[index]
                    powerInAtCompression_dBm.last());
            qDebug() << "Pout (compression): " << powerOutAtCompression_dBm.last() << " dBm";
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
            qDebug() << "Missing compression point for frequency " << formatValue(frequencies_Hz[iFreq], 3, Units::Hertz);
        }
    }
}

// Export
void MeasurementData::exportTouchstone(QString path) {
    for (uint i = 0; i < powerPoints; i++) {
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
    for (uint i = 0; i < frequencyPoints; i++) {
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
               >> ifBw_Hz
               >> frequencyPoints
               >> powerPoints
               >> frequencies_Hz
               >> power_dBm
               >> s11_dB
               >> s22_dB
               >> gain_dB
               >> powerOut_dBm
               >> referenceGain_dB
               >> compressionLevel_dB
               >> powerInAtCompression_dBm
               >> powerOutAtCompression_dBm
               >> compressionFrequencies_Hz;
        data.resize(powerPoints);
        for (uint i = 0; i < powerPoints; i++) {
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
               << ifBw_Hz
               << frequencyPoints
               << powerPoints
               << frequencies_Hz
               << power_dBm
               << s11_dB
               << s22_dB
               << gain_dB
               << powerOut_dBm
               << referenceGain_dB
               << compressionLevel_dB
               << powerInAtCompression_dBm
               << powerOutAtCompression_dBm
               << compressionFrequencies_Hz;
        for (uint i = 0; i < powerPoints; i++) {
            stream << data[i];
        }
        return true;
    }
    catch(void *) {
        return false;
    }
}
