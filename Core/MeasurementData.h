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
    RsaToolbox::QRowVector &pin_dBm();

    RsaToolbox::QRowVector &powerInAtMaxGain_dBm();
    RsaToolbox::QRowVector &maxGain_dB();
    RsaToolbox::ComplexMatrix3D &sParametersAtMaxGain();
    RsaToolbox::QRowVector &powerOutAtMaxGain_dBm();
    RsaToolbox::QRowVector &powerInAtCompression_dBm();
    RsaToolbox::QRowVector &gainAtCompression_dB();
    RsaToolbox::ComplexMatrix3D &sParametersAtCompression();
    RsaToolbox::QRowVector &powerOutAtCompression_dBm();

    bool sParameterVsPin(double frequency_Hz, uint outputPort, uint inputPort, RsaToolbox::QRowVector &pin_dBm, RsaToolbox::ComplexRowVector &sParameter);
    bool sParameterVsPout(double frequency_Hz, uint outputPort, uint inputPort, RsaToolbox::QRowVector &pout_dBm, RsaToolbox::ComplexRowVector &sParameter); // New!
    RsaToolbox::ComplexRowVector sParameterAtCompression(uint outputPort, uint inputPort);
    RsaToolbox::ComplexRowVector sParameterAtMaxGain(uint outputPort, uint inputPort);
    bool sParameterVsFrequencyAtPin(double pin_dBm, uint outputPort, uint inputPort, RsaToolbox::QRowVector &frequencies_Hz, RsaToolbox::ComplexRowVector &sParameter);
//    bool sParameterVsFrequencyAtPout(double pout_dBm, uint outputPort, uint inputPort, RsaToolbox::QRowVector &frequencies_Hz, RsaToolbox::ComplexRowVector &sParameter);
    // ... This would involve major interpolation effort?

    bool poutVsFrequency(double pin_dBm, RsaToolbox::QRowVector &frequencies_Hz, RsaToolbox::QRowVector &pout_dBm);
    bool poutVsPin(double frequency_Hz, RsaToolbox::QRowVector &pin_dBm, RsaToolbox::QRowVector &pout_dBm);

    // measuredPowers_dBm()[iPower][iFreq]
    QVector<RsaToolbox::QRowVector> &measuredPin_dBm();

    // data[power]->y()[freq][outputPort-1][inputPort-1]
    // where inputPort, outputPort => [1,2]
    QVector<RsaToolbox::NetworkData> &data();

    void clearAllData();

    bool open(QString filename);
    bool save(QString filename);

    bool exportToZip(QString filename);


private:
    QString _appName;
    QString _appVersion;
    QDateTime _timeStamp;
    QString _vnaInfo;
    MeasurementSettings _settings;

    RsaToolbox::QRowVector _frequencies_Hz;
    RsaToolbox::QRowVector _pin_dBm;

    RsaToolbox::QRowVector _powerInAtMaxGain_dBm;
    RsaToolbox::QRowVector _maxGain_dB;
    RsaToolbox::ComplexMatrix3D _sParametersAtMaxGain;
    RsaToolbox::QRowVector _powerOutAtMaxGain_dBm;
    RsaToolbox::QRowVector _powerInAtCompression_dBm;
    RsaToolbox::QRowVector _gainAtCompression_dB;
    RsaToolbox::ComplexMatrix3D _sParametersAtCompression;
    RsaToolbox::QRowVector _powerOutAtCompression_dBm;

    QVector<RsaToolbox::QRowVector> _measuredPin_dBm;
    QVector<RsaToolbox::NetworkData> _data;

    // Keep?
    bool exportInfo(QString path);
    bool exportSimpleCsv(QString path);
    bool exportCompleteCsv(QString path);
    bool exportTouchstone(QString path);

    QString toScientificNotationWithComma(const double value);
};

#endif // MEASUREMENTDATA_H
