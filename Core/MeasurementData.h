#ifndef MEASUREMENTDATA_H
#define MEASUREMENTDATA_H


// Project
#include "MeasurementSettings.h"

// RsaToolbox
#include <Definitions.h>
#include <NetworkData.h>
#include <Vna.h>

// Qt
#include <QObject>
#include <QDateTime>
#include <QDataStream>


class MeasurementData : public QObject
{
    Q_OBJECT
public:
    explicit MeasurementData(QObject *parent = 0);
    ~MeasurementData();

    // General
    QString applicationName,
            applicationVersion;

    QDateTime timeStamp;
    QString vnaMake, vnaModel,
            vnaSerialNo, vnaFirmwareVersion;

    // Settings
    bool isValidSettings(RsaToolbox::Vna &vna) const;
    bool isValidSettings(RsaToolbox::Vna &vna, QString &errorMessage) const;
    MeasurementSettings settings() const;
    MeasurementSettings &settings();
    void setSettings(const MeasurementSettings &settings);

    // Initialize
    void processSettings();
    RsaToolbox::QRowVector frequencies_Hz;
    RsaToolbox::QRowVector power_dBm;

    // Measure
    void resizeToPoints();
    QVector<RsaToolbox::NetworkData> data; // data[power]->S[freq][output][input])

    // Calculate
    void calculateMetrics();
    RsaToolbox::QRowVector s11_dB; // at Reference (Max) gain
    RsaToolbox::QRowVector s22_dB; // at Reference (Max) gain
    RsaToolbox::QMatrix2D gain_dB; // [freq][power]
    RsaToolbox::QMatrix2D powerOut_dBm; // [freq][power]

    QVector<int> referenceGainIndexes; // [Freq]
    RsaToolbox::QRowVector referenceGain_dB; // [Freq]
    RsaToolbox::ComplexMatrix3D s_referenceGain; // S[Freq] @ Max gain

    RsaToolbox::QRowVector powerInAtCompression_dBm; // [Freq]
    RsaToolbox::QRowVector powerOutAtCompression_dBm; // [Freq]
    RsaToolbox::QRowVector compressionFrequencies_Hz; // ?
    RsaToolbox::ComplexMatrix3D s_compression; // S[Freq] @ Compression

    // File
    bool open(QString filename);
    bool save(QString filename);
    bool exportToZip(QString filename);

signals:
    void dataReset();

public slots:
    void reset();
    void emitReset();

private:
    MeasurementSettings _settings;
    void resetSettings();
    void resetData();

    void processReflectionCoefficients();
    void processGain();
    void processPowerOut();
    void findMaximumGain();
    void findCompressionPoints();

    void exportTouchstone(QString path);
    void exportReferenceGain(QString filename);
    void exportCompressionPoints(QString filename);

    bool open(QDataStream &stream);
    bool save(QDataStream &stream);
};

#endif // MEASUREMENTDATA_H
