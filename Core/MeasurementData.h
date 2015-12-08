#ifndef MEASUREMENTDATA_H
#define MEASUREMENTDATA_H


// Project
#include "MeasurementSettings.h"

// RsaToolbox
#include <Definitions.h>
#include <NetworkData.h>
#include <Vna.h>

// Qt
#include <QVector>
#include <QDateTime>
#include <QDataStream>


class MeasurementData
{
public:
    explicit MeasurementData();
    ~MeasurementData();

    void setAppInfo(const QString &name, const QString &version);
    void setTimeToNow();
    void setVnaInfo(RsaToolbox::Vna &vna);

    MeasurementSettings settings() const;
    void setSettings(const MeasurementSettings &settings);


    uint frequencyPoints() const;
    RsaToolbox::QRowVector &frequencies_Hz();

    uint powerPoints() const;
    RsaToolbox::QRowVector &powers_dBm();

    RsaToolbox::QRowVector &powerInAtMaxGain_dBm();
    RsaToolbox::QRowVector &maxGain_dB();
    RsaToolbox::QRowVector &powerOutAtMaxGain_dBm();
    RsaToolbox::QRowVector &powerInAtCompression_dBm();
    RsaToolbox::QRowVector &gainAtCompression_dB();
    RsaToolbox::QRowVector &powerOutAtCompression_dBm();

    RsaToolbox::ComplexRowVector sParameterAtCompression(uint outputPort, uint inputPort);
    RsaToolbox::ComplexRowVector sParameterAtMaxGain(uint outputPort, uint inputPort);

    void sParameterVsPower(double frequency_Hz, uint outputPort, uint inputPort, RsaToolbox::QRowVector &powers_dBm, RsaToolbox::ComplexRowVector &sParameter);
    void sParameterVsFrequency(double power_dBm, uint outputPort, uint inputPort, RsaToolbox::QRowVector &frequencies_Hz, RsaToolbox::ComplexRowVector &sParameter);

    // data[power]->y()[freq][outputPort-1][inputPort-1]
    // where inputPort, outputPort => [1,2]
    QVector<RsaToolbox::NetworkData> &data();

    void clearAllData();

    bool exportToZip(QString filename);

private:
    QString _appName;
    QString _appVersion;
    QDateTime _timeStamp;
    QString _vnaInfo;
    MeasurementSettings _settings;

    RsaToolbox::QRowVector _frequencies_Hz;
    RsaToolbox::QRowVector _powers_dBm;

    RsaToolbox::QRowVector _powerInAtMaxGain_dBm;
    RsaToolbox::QRowVector _maxGain_dB;
    RsaToolbox::QRowVector _powerOutAtMaxGain_dBm;
    RsaToolbox::QRowVector _powerInAtCompression_dBm;
    RsaToolbox::QRowVector _gainAtCompression_dB;
    RsaToolbox::QRowVector _powerOutAtCompression_dBm;

    QVector<RsaToolbox::NetworkData> _data;

    RsaToolbox::ComplexMatrix3D sParametersAtCompression();
    RsaToolbox::ComplexMatrix3D sParametersAtMaxGain();

    // Keep?
    bool exportInfo(QString path);
    bool exportCsv(QString path);
    bool exportTouchstone(QString path);
};

#endif // MEASUREMENTDATA_H
